Drupal.behaviors.TZSMSUserOverview = function(context) {

    function clearAllCheckBoxes() {
        $('tbody :checked').removeAttr('checked').click();
    }

    function showTextSMSDialog(text) {
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

            runWithProgressBar(employees, 5, function(employees, on_success) {
                sendTextSMS(text, employees, on_success);
            });
        };

        dialog.dialog({
            buttons: buttons,
            width: "450px"
        });
    }

    function selectedEmployees() {
        var employees = [];
        $('table.sticky-table :checked').each(function(index, element) {
            employees.push($(element).val());
        });
        return employees;
    }

    function runWithProgressBar(employees, chunk_size, on_process) {
        var dialog,
            progressbar,
            info;

        $('#send-sms-progress-dialog').remove();
        dialog = $('<div id="send-sms-progress-dialog" title="' + Drupal.t('Sending') + '..."></div>');

        progressbar = $('<div></div>');
        dialog.append(progressbar);
        progressbar.progressbar();

        info = $('<div class="messages status"></div>');
        dialog.append(info);

        dialog.dialog({
            modal: true
        });

        function processEmployeeChunks() {
            var sent = 0,
                total = employees.length;

            function nextEmployeeChunk() {
                sent += chunk_size;
                sent = Math.min(total, sent);
                progressbar.progressbar('value', sent*100/total);
                info.text(Drupal.t('Sent @count of @total', {
                    "@count": sent,
                    "@total": total
                }));

                if (sent < total) {
                    on_process(employees.slice(sent, sent + chunk_size), nextEmployeeChunk);
                } else {
                    clearAllCheckBoxes();
                    setTimeout(function() {
                        dialog.dialog('close');
                        dialog.remove();
                    }, 1000);
                }
            }

            // Slice employees into chunks of 5
            on_process(employees.slice(sent, sent + chunk_size), nextEmployeeChunk);
        }

        processEmployeeChunks();
    }

    function sendTextSMS(text, employees, on_success) {
        $.post('tzsms/send_text_sms_ajax', {
            "text": text,
            "selected_users[]": employees
        }, on_success);
    }

    function sendInstallSMS(employees, on_success) {
        $.post('tzsms/install_sms_ajax', {
            "selected_users[]": employees
        }, on_success);
    }

    $('#edit-send-install-sms').click(function(event) {
        var employees = selectedEmployees();

        event.preventDefault();

        if (employees.length > 0) {
            runWithProgressBar(employees, 5, sendInstallSMS);
        }
    });

    $('#edit-send-reminder-sms').click(function(event) {
        event.preventDefault();
        showTextSMSDialog(Drupal.t('Hi! We are waiting for some of your time reports, please fill them in.'))
    });

    $('#edit-send-text-sms').click(function(event) {
        event.preventDefault();
        showTextSMSDialog();
    });

};