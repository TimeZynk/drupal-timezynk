(function($) {
	$.distance_of_time_in_words = function(timestamp) {
		var interval,
			seconds = (new Date()).getTime() / 1000 - timestamp;

		if (timestamp === 0) {
			return Drupal.t('never');
		}

		return Drupal.t('@time ago', {'@time': format_interval(seconds)});
	};

	function format_interval(seconds) {
		if (seconds === 0) {
			return Drupal.t('0 sec');
		} else if (seconds >= 31536000) {
			return Drupal.formatPlural(Math.floor(seconds/31536000), '1 year', '@count years');
		} else if (seconds >= 604800) {
			return Drupal.formatPlural(Math.floor(seconds/604800), '1 week', '@count weeks');
		} else if (seconds >= 86400) {
			return Drupal.formatPlural(Math.floor(seconds/86400), '1 day', '@count days');
		} else if (seconds >= 3600) {
			return Drupal.formatPlural(Math.floor(seconds/3600), '1 hour', '@count hours');
		} else if (seconds >= 60) {
			return Drupal.formatPlural(Math.floor(seconds/60), '1 min', '@count min');
		} else {
			return Drupal.formatPlural(Math.floor(seconds), '1 sec', '@count sec');
		}
	}

	$.format_short_date = function(timestamp) {
		var d = new Date(timestamp * 1000)
			s = d.getFullYear() + '-';

		if (d.getMonth() < 9) {
			s += '0';
		}
		s += (d.getMonth() + 1) + '-';

		if (d.getDate() < 10) {
			s += '0';
		}
		s += d.getDate();

		return s;
	};
})(jQuery);
