Drupal.behaviors.TZUserOverview = function(context) {
    var sortBy = [Drupal.t('Full name'), 'asc'],
        comparator,
        data = [],
        selections = {},
        lastClickedCheckboxID = null,
        permissions = {},
        refreshIntervalId;

    function makeFieldComparator(field) {
        return function(a, b) {
            if (a[field] > b[field]) {
                return 1;
            }

            if (a[field] < b[field]) {
                return -1;
            }

            /* The field is equal in both objects
             * Try to fallback to comparing the name
             * since it should always be unique.
             */
            if (field != 'name_value') {
                return makeFieldComparator('name_value')(a,b);
            }
            return 0;
        };
    }

    function reverse(fn) {
        return function(a,b) {
            return fn(b,a);
        };
    }

    function updateSortBy(hash) {
        if (hash) {
            sortBy = hash.substring(1).split(';');
        }
        switch (sortBy[0]) {
            case Drupal.t('Status'):
                comparator = makeFieldComparator('status');
                break;
            case Drupal.t('Username'):
                comparator = makeFieldComparator('username');
                break;
            case Drupal.t('Full name'):
                comparator = makeFieldComparator('fullname');
                break;
            case Drupal.t('Mobile'):
                comparator = makeFieldComparator('mobile');
                break;
            case Drupal.t('Due reports'):
                comparator = makeFieldComparator('due_count');
                break;
            case Drupal.t('Created'):
                comparator = makeFieldComparator('created');
                break;
            case Drupal.t('Last login'):
                comparator = makeFieldComparator('last_login');
                break;
            default:
                comparator = makeFieldComparator('fullname');
                break;
        }

        if (sortBy[1] != 'asc') {
            comparator = reverse(comparator);
        }
    }

    function setTableSortLinks() {
        $('.sticky-table thead th').each(function(index, element) {
            var text = $(element).text();
            var anchor = '#' + encodeURI(text) + ';';
            if (text === sortBy[0]) {
                anchor += sortBy[1] == 'asc' ? 'des' : 'asc';
            } else {
                anchor += 'asc';
            }
            var href = $('<a class="active" href="' + anchor + '">' + text + '</a>');
            href.click(function(event) {
                var hash = decodeURI($(this).attr('href'));
                updateSortBy(hash);
                renderOverviewTable();
            });
            $(element).html(href);
        });
    }

    function connectFilters() {
        $('.form-checkboxes input').click(function(event) {
            startOverviewRefreshCycle();
        });
        $('#edit-manager').change(function(event) {
            startOverviewRefreshCycle();
        });
        $('#edit-date-datepicker-popup-0').change(function(event) {
            startOverviewRefreshCycle();
        });
    }

    function renderOverviewTable() {
        setTableSortLinks();
        if(!data) {
            return;
        }
        data.sort(comparator);
        var tableBody = $('.sticky-table tbody');
        tableBody.html('');
        var rowCount = 0;
        for (var uid in data) {
            var user = data[uid],
                row = $('<tr></tr>').addClass(rowCount & 1 ? 'odd' : 'even'),
                operations = $('<td class="user_operations"></td>');

            $.each([
                renderCheckbox(user),
                renderTrafficLight(user),
                renderUserName(user),
                '<td>' + user.fullname + '</td>',
                '<td>' + user.mobile + '</td>',
                renderDueReports(user),
                renderCreated(user),
                renderLastLogin(user),
                operations
            ], function(i, e) { row.append(e); });

            tableBody.append(row);
            rowCount++;
        }

        // Add count row
        var countRow = $('<tr>' +
            '<td>&nbsp;</td><td>' + Drupal.t('Total') + '</td>' +
            '<td>' + rowCount + '</td>' +
            '<td colspan="6">&nbsp;</td>' +
            '</tr>');
        countRow.addClass('tzuser-overview-count-row');
        tableBody.append(countRow);

        bindCheckboxes(tableBody);
        fillOperations(tableBody);
        $('.ahah-progress').html('');
    }

    function renderCheckbox(user) {
        var id = user.id;
        return '<td><div class="form-item" id="selected_users_' + id + '-wrapper">' +
            '<input type="checkbox" name="selected_users[' + id + ']" id="selected_users_' + id +
            '" value="' + id + '" class="form-checkbox"></div></td>';
    }

    function renderTrafficLight(user) {
        return '<td><div class="tzuser_status tzuser_status_' + user.status_name + '"></div></td>';
    }

    function renderUserName(user) {
        return '<td><a href="/user/' + user.id + '?destination=tzuser">' + user.username + '</a></td>';
    }

    function renderDueReports(user) {
        if (user.due_count >= 0) {
            return '<td><span class="due_reports_count">' + user.due_count + '</span></td>';
        }
        return '<td>' + Drupal.t('Unknown') + '</td>';
    }

    function renderLastLogin(user) {
        return '<td>' + $.distance_of_time_in_words(user.last_login) + '</td>';
    }

    function renderCreated(user) {
        return '<td>' + $.format_short_date(user.created) + '</td>';
    }

    function fillOperations(tableBody) {
        if (typeof(permissions['administer users']) === 'undefined' ||
            typeof(permissions['administer site configuration']) === 'undefined') {
            $.getJSON('/api/access', {
                'permission[0]': 'administer users',
                'permission[1]': 'administer site configuration'
            }, function(data) {
                $.extend(permissions, data);
                fillOperations(tableBody);
            });
            return;
        }
        tableBody.find('.user_operations').each(function() {
            var that = $(this),
                row = that.parents('tr'),
                uid = row.find('input.form-checkbox').val();

            if (permissions['administer users']) {
                that.append('<a href="/user/' + uid + '/edit?destination=tzuser">' + Drupal.t('edit') + '</a>');
            }

            if (permissions['administer site configuration']) {
                var link = $('<a href="#">' + Drupal.t('log') + '</a>');
                link.editSupportLogLink(function() { return uid; });
                if (that.size()) {
                    that.append(' | ');
                }
                that.append(link);
            }
        });
    }

    function bindCheckboxes(tableBody) {
        var checkboxes = tableBody.find(':checkbox');

        /* Enable shift-selecting checkboxes and store
         * checkbox state over page refreshes.
         */
        checkboxes.click(function(event) {
            var that = $(this),
                lastClickedCheckbox,
                lastClickedStatus,
                lastClickedIndex,
                thisIndex,
                affectedCheckboxes;

            selections[that.val()] = that.attr('checked') ? true : false;

            if (lastClickedCheckboxID && event.shiftKey) {
                /* Shift key pressed, find all checkboxes between
                 * the last one and this one and select them too.
                 */
                lastClickedCheckbox = $(lastClickedCheckboxID);
                lastClickedIndex = checkboxes.index(lastClickedCheckbox);
                lastClickedStatus = lastClickedCheckbox.attr('checked') ? true : false
                thisIndex = checkboxes.index(that);

                affectedCheckboxes = checkboxes.slice(Math.min(lastClickedIndex, thisIndex),
                    Math.max(lastClickedIndex, thisIndex) + 1);
                affectedCheckboxes.attr('checked', lastClickedStatus ? 'checked' : '');

                // Store selection for page reloads
                affectedCheckboxes.each(function(i, element) {
                    selections[$(element).val()] = lastClickedStatus;
                });
            }

            lastClickedCheckboxID = '#' + that.attr('id');
        });

        /* Restore selections from last selected state */
        checkboxes.each(function(i, element) {
            var checkbox = $(element);
            if (selections[checkbox.val()]) {
                checkbox.attr('checked', 'checked');
            } else {
                checkbox.removeAttr('checked');
            }
        });
    }

    function updateOverviewData() {
        var url = 'api/users',
            form_elements = $('#tzuser-user-overview fieldset:first :input');

        url += '?' + form_elements.serialize();

        $('.ahah-progress').html('<div class="throbber"></div>');
        $.ajax({
            url: url,
            dataType: 'json',
            success: function (jsonData) {
                data = jsonData;
                renderOverviewTable();
            },
            error: function () {
                window.location.reload();
            }
        });
    }

    function startOverviewRefreshCycle() {
		var refreshInterval = 30000;

        if (refreshIntervalId) {
            clearInterval(refreshIntervalId);
        }

		if (Drupal.settings.tzuser.OVERVIEW_REFRESH_RATE) {
			refreshInterval = Drupal.settings.tzuser.OVERVIEW_REFRESH_RATE * 1000;
		}

        updateOverviewData();
        refreshIntervalId = setInterval(updateOverviewData, refreshInterval);
    }

    function selectedEmployees() {
        var employee_map = {},
            employees = [],
            key;

        $('table td .form-item :checked').each(function(index, element) {
            employee_map[$(element).val()] = $(element).val();
        });
        for (key in employee_map) {
            employees.push(key);
        }
        return employees;
    }

    function deleteUsers(employees, on_success) {
        console.log('/api/users/' + employees[0]);
        $.ajax({
            url: '/api/users/' + employees[0],
            type: "DELETE",
            success: on_success
        });
    }

    $('#edit-delete-user').click(function(event) {
        var employees = selectedEmployees();
        console.log(employees);
        event.preventDefault();
        if (employees.length > 0) {
            $.runWithProgressBar(employees, {
                chunk_size: 1,
                on_process: deleteUsers,
                on_finished: startOverviewRefreshCycle
            });
        }
    });

    connectFilters();
    updateSortBy(window.location.hash);
    startOverviewRefreshCycle();
};
