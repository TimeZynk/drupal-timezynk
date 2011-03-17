Drupal.behaviors.TZReportsTimeReports = function(context) {
    function showTimeReports(response) {
       $('.ahah-progress').html('');
       $('#timereports-wrapper').html(response);
    }

    function refreshTimeReports() {
        var formdata = $('#tzreports-timereports').serialize();
        $('.ahah-progress').html('<div class="throbber"></div>');
        $.post('timereports/all/ajax', formdata, showTimeReports);
    }

    function bindHandlers() {
        $('input').change(function (event) {
            refreshTimeReports();
        });
        $('select').change(function (event) {
            refreshTimeReports();
        });
    }

    bindHandlers();
    refreshTimeReports();
};