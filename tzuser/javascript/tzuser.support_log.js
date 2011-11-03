(function($) {
    $.fn.editSupportLogLink = function(uidfn, on_success) {
        var that = this,
            hash = location.hash;

        if (!hash) {
            hash = '#';
        }

        that.attr('href', hash).click(function(event) {
            var uid = uidfn;
            event.preventDefault();
            if ($.isFunction(uidfn)) {
                uid = uidfn.call($(this));
            }
            $.ajax({
                url: '/tzuser/support_log/' + uid,
                dataType: 'json',
                success: function (jsonData) {
                    showEditLogDialog(jsonData, on_success);
                }
            });
        });
    }

    function showEditLogDialog(logData, on_success) {
        var dialog,
            errors,
            form,
            textarea,
            buttons = {};

        $('#edit-support-log-dialog').remove();
        dialog = $('<div id="edit-support-log-dialog" title="' + Drupal.t('Edit support log') + '"></div>');

        errors = $('<div class="messages error"></div>');
        errors.hide();
        dialog.append(errors);

        form = $('<form></form>');

        textarea = $('<textarea cols="80" rows="10" name="text" class="ui-widget-content ui-corner-all"></textarea>');
        if (logData.support_log) {
            textarea.val(logData.support_log);
        }
        form.append(textarea);
        dialog.append(form);
        $('body').append(dialog);
        dialog.hide();

        buttons[Drupal.t('Save')] = function() {
            var data = {
                uid: logData.uid,
                support_log: textarea.val()
            };

            dialog.dialog('close');
            dialog.remove();
            $.post('/tzuser/support_log/' + data.uid, data, on_success);
        };

        dialog.dialog({
            buttons: buttons,
            width: "650px"
        });
    };
})(jQuery);
