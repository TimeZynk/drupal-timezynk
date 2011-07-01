Drupal.behaviors.Availability = function(context) {
	function init() {
		$('#edit-from').timeEntry({show24Hours: true});
		$('#edit-to').timeEntry({show24Hours: true});
		$('#edit-submit').click(function(event) {
	        event.preventDefault();
	        var form_data = $('#tzbase-availability-form fieldset').serialize();
	        $.get('availability/data', form_data, displayData);
	    });
	}
	
	function displayData(data) {
		$('#availability-data').html(data);
	}
	
	init();
};