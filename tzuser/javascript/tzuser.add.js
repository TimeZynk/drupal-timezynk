$(function() {
	var entries = Drupal.settings.tzuser_new_entries,
	    results = {};

	$.runWithProgressBar(entries, {
		chunk_size: 5,
		on_process: add_users,
		on_finished: add_users_finished,
		title: Drupal.t('Adding employees') + '...'
	});

	function add_users(users, on_success) {
		$.post('/api/users', JSON.stringify(users), function(data, textStatus) {
			$.extend(results, data);
			on_success();
		}, 'json');
	}

	function add_users_finished() {
		var messages = [],
			errors = [];

		for(var key in results) {
			if (results[key].message) {
				messages.push(results[key].message);
			} else if (results[key].error) {
				errors.push(results[key].error);
			}
		}

		if (messages.length) {
			$('#tzuser-add-users-form > div').prepend('<div class="messages status">' + messages.join('<br/>') + '</div>');
		}
		if (errors.length) {
			$('#tzuser-add-users-form > div').prepend('<div class="messages error">' + errors.join('<br/>') + '</div>');
		}

		// Clear all input fields
		$('input.form-text').val('');
		$('select.form-select').val(0);
	}
});
