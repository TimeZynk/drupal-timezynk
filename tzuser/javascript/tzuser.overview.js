Drupal.behaviors.TZUserOverview = function(context) {
    var sortBy = [Drupal.t('Full name'), 'asc'];
    var showFilter = {
        '20': true,
        '10': true,
        '0': true
    };
    var comparator;
    var data = [];
    var selections = {};

    function makeFieldComparator(field) {
        return function(a, b) {
            return a[field] >= b[field];
        }
    }

    function reverse(fn) {
        return function(a,b) {
            return fn(b,a);
        }
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
                comparator = makeFieldComparator('name');
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

    function shouldShow(row) {
        return showFilter[row['status_value']];
    }

    function connectCheckboxes() {
        $('.form-checkboxes input').change(function(event) {
            var that = $(this);
            showFilter[that.val()] = that.attr('checked') ? true : false;
            renderOverviewTable();
        });
    }

    function renderOverviewTable() {
        setTableSortLinks();
        if(!data) {
            return;
        }
        data.sort(comparator);
        tableBody = $('.sticky-table tbody');
        tableBody.html('');
        var rowCount = 0;
        for (var uid in data) {
            var row = $('<tr></tr>');
            row.addClass(rowCount & 1 ? 'odd' : 'even');

            var dataRow = data[uid];
            if (!shouldShow(dataRow)) {
                continue;
            }

            for (var field in dataRow) {
                if (field.match(/_value/)) {
                    continue;
                }
                row.append('<td>' + dataRow[field] + '</td>');
            }
            tableBody.append(row);
            rowCount++;
        }

        // Keep selections over table reloads
        tableBody.find(':checkbox').change(function() {
            var that = $(this);
            selections[that.val()] = that.attr('checked');
        }).each(function(i, element) {
            element = $(element);
            if (selections[element.val()]) {
                element.attr('checked', 'checked');
            } else {
                element.removeAttr('checked');
            }
        });

    }

    function updateOverviewData() {
        $.getJSON('tzuser/overview/ajax',
            '',
            function (jsonData) {
                data = jsonData;
                renderOverviewTable();
            }
        );
    }

    connectCheckboxes();
    updateSortBy(window.location.hash);
    updateOverviewData();
    setInterval(updateOverviewData, 5000);
};