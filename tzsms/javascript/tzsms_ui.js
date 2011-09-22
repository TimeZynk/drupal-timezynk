(function($) {
	$.showTextSMSDialog = function(text, selectedEmployees) {
        var dialog,
            errors,
            form,
            textarea,
            length_info,
            buttons = {};

        $('#send-text-sms-dialog').remove();
        dialog = $('<div id="send-text-sms-dialog" title="' + Drupal.t('Send text-SMS') + '"></div>');

        errors = $('<div class="messages error"></div>');
        errors.hide();
        dialog.append(errors);

        form = $('<form></form>');

        textarea = $('<textarea cols="50" rows="4" name="text" class="ui-widget-content ui-corner-all"></textarea>');
        if (text) {
            textarea.val(text);
        }
        form.append(textarea);

        length_info = $('<div id="send-text-sms-info" class="info"></div>');
        form.append(length_info);

        textarea.keyup(function() {
            length_info.text(
                Drupal.t('Used @count of 459 characters. (@count_sms SMS-messages)',
                {
                    "@count": textarea.val().length,
                    "@count_sms": Math.ceil(textarea.val().length/160)
                }));
        });
        textarea.keyup();

        dialog.append(form);
        $('body').append(dialog);
        dialog.hide();

        buttons[Drupal.t('Send')] = function() {
            var employees = selectedEmployees(),
                text = textarea.val();

            if (employees.length === 0) {
                errors.text(Drupal.t('No employees selected')).show();
                return;
            }

            if (text.length === 0) {
                errors.text(Drupal.t('Empty SMS message')).show();
                return;
            }

            if (text.length > 459) {
                errors.text(Drupal.t('SMS too long')).show();
                return;
            }

            errors.text('').hide();
            dialog.dialog('close');
            dialog.remove();

            $.runWithProgressBar(employees, {
            	chunk_size: 5,
            	on_process: function(employees, on_success) {
                    sendTextSMS(text, employees, on_success);
                },
                on_finished: clearAllCheckBoxes
            });
        };

        dialog.dialog({
            buttons: buttons,
            width: "450px"
        });
    };
    
    function clearAllCheckBoxes() {
        $('tbody :checked').removeAttr('checked').click();
    }
    
    function sendTextSMS(text, employees, on_success) {
        $.post('tzsms/send_text_sms_ajax', {
            "text": text,
            "selected_users[]": employees
        }, on_success);
    }
    
})(jQuery);