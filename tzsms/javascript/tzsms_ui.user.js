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
        $.post('tzsms/install_sms_ajax', {
            "selected_users[]": employees
        }, on_success);
    }

    $('#edit-send-install-sms').click(function(event) {
        var employees = selectedEmployees();

        event.preventDefault();

        if (employees.length > 0) {
            $.runWithProgressBar(employees, 5, sendInstallSMS);
        }
    });

    $('#edit-send-reminder-sms').click(function(event) {
        event.preventDefault();
        $.showTextSMSDialog(Drupal.t('Hi! We are waiting for some of your time reports, please fill them in.'), selectedEmployees);
    });

    $('#edit-send-text-sms').click(function(event) {
        event.preventDefault();
        $.showTextSMSDialog('', selectedEmployees);
    });
};