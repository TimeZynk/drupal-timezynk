Drupal.behaviors.TZSMSUserOverview = function(context) {
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

    function sendInstallSMS(employees, on_success) {
        $.post('/tzsms/install_sms_ajax', {
            "selected_users[]": employees
        }, on_success);
    }

    function clearAllCheckBoxes() {
        $('tbody :checked').removeAttr('checked').click();
    }

    $('#edit-send-install-sms').click(function(event) {
        var employees = selectedEmployees();

        event.preventDefault();

        if (employees.length > 0) {
            $.runWithProgressBar(employees, {
                chunk_size: 5,
                on_process: sendInstallSMS,
                on_finished: clearAllCheckBoxes
            });
        }
    });

    $('#edit-send-reminder-sms').click(function(event) {
        event.preventDefault();
        $.showTextSMSDialog(Drupal.settings.tzsms_ui.tzsms_reminder_sms_template, selectedEmployees);
    });

    $('#edit-send-text-sms').click(function(event) {
        event.preventDefault();
        $.showTextSMSDialog('', selectedEmployees);
    });
};