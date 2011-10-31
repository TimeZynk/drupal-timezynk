Drupal.behaviors.TZUserOverview = function(context) {
    var sortBy = [Drupal.t('Full name'), 'asc'],
        comparator,
        data = [],
        selections = {},
        lastClickedCheckboxID = null,
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
                comparator = makeFieldComparator('status_value');
                break;
            case Drupal.t('Username'):
                comparator = makeFieldComparator('name_value');
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
            case Drupal.t('Last login'):
                comparator = makeFieldComparator('login_value');
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
            var row = $('<tr></tr>');
            row.addClass(rowCount & 1 ? 'odd' : 'even');

            var dataRow = data[uid];

            for (var field in dataRow) {
                if (field.match(/_value/)) {
                    continue;
                }
                row.append('<td>' + dataRow[field] + '</td>');
            }
            tableBody.append(row);
            rowCount++;
        }

        // Add count row
        var countRow = $('<tr>' +
            '<td>&nbsp;</td><td>' + Drupal.t('Total') + '</td>' +
            '<td>' + rowCount + '</td>' +
            '<td colspan="5">&nbsp;</td>' +
            '</tr>');
        countRow.addClass('tzuser-overview-count-row');
        tableBody.append(countRow);

        bindCheckboxes(tableBody);
        bindEditLogLinks(tableBody);
        $('.ahah-progress').html('');
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

    function bindEditLogLinks(tableBody) {
        tableBody.find('.edit-log-link').editSupportLogLink(function() {
            var that = $(this),
                row = that.parents('tr'),
                uid = row.find('input.form-checkbox').val();
            return uid;
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

    connectFilters();
    updateSortBy(window.location.hash);
    startOverviewRefreshCycle();
};
