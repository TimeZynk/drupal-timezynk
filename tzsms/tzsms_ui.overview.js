Drupal.behaviors.TZSMSUserOverview = function(context) {

    function displayNotice(message) {
        // Show message
        $('div.messages').remove();
        message.hide();
        $('form > div').prepend(message);
        message.fadeIn();
    }

    function postCheckboxes(url) {
        var handleResponse = function(response) {
            displayNotice($(response));
            // Clear selections
            $('tbody :checked').removeAttr('checked').change();
        };
        var formData = $('#tzuser-user-overview').serialize();

        $.post(url, formData, handleResponse);
    }

    $('#edit-send-install-sms').click(function(event) {
        event.preventDefault();
        postCheckboxes('tzsms/install_sms_ajax');
    });

    $('#edit-send-reminder-sms').click(function(event) {
        event.preventDefault();
        postCheckboxes('tzsms/reminder_sms_ajax');
    });
}