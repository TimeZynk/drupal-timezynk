Drupal.behaviors.TZSMSUserOverview = function(context) {
    $('#edit-send-install-sms').click(function (event) {
        event.preventDefault();
        $.post('tzsms/install_sms_ajax',
            $('#tzuser-user-overview').serialize(),
            function(response) {
                // Show message
                $('div.messages').remove();
                var message = $(response);
                message.hide();
                $('form > div').prepend(message);
                message.fadeIn();

                // Clear selections
                $('tbody :checked').removeAttr('checked').change();
            }
        );
    });
}