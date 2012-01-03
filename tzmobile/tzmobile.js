var tzmobile = {
		'history': [],
		'images': [],
		'jobs': {
			1: {'name': 'Butikservice', 'parent': 0},
			2: {'name': 'Pilotprojekt', 'parent': 1},
			3: {'name': 'Utveckling', 'parent': 1},
			11: {'name': 'WÅ Bygg', 'parent': 0},
			12: {'name': 'Pilotprojekt', 'parent': 11},
			13: {'name': 'Utveckling', 'parent': 11},
			21: {'name': 'Effekt Personal', 'parent': 0},
			22: {'name': 'Pilotprojekt', 'parent': 21},
			23: {'name': 'Utveckling', 'parent': 21}
		},
		'team': [
			{
				username: '0708194831',
				realname: 'Lisa Alerstam',
				image: 'lisa_alerstam.jpg',
				today: 4*3600,
				week: 4*8*3600,
				month: 15*6*3600
			},
			{
				username: '0702897764',
				realname: 'Mikael Ohlson',
				image: 'mikael_ohlson.jpg',
				today: 3*3600+1800,
				week: 4*7*3600+900,
				month: 15*7*3600+1800
			},
			{
				username: '0734434150',
				realname: 'Ulf Jönsson',
				image: 'ulf_jonsson.jpg',
				today: 0,
				week: 3*8*3600,
				month: 15*6*3600
			},
		],
		'report': {
			'job': 0
		}
};

Drupal.behaviors.tzmobileBehavior = function(context) {
	// Find topmost frame
	var frame = $('#block-' + Drupal.settings.tzmobile.blockid);
	if(frame.length > 0) {
		tzmobile.frame = $(frame.get(0));

		tzmobile_load(encodeURIComponent('tzmobile_main_menu()'), Drupal.t('Home'));
	}

	// Set datepicker region
    $.datepicker.regional['sv'] = {
    		closeText: 'Stäng',
            prevText: '&laquo;Förra',
    		nextText: 'Nästa&raquo;',
    		currentText: 'Idag',
            monthNames: ['Januari','Februari','Mars','April','Maj','Juni',
            'Juli','Augusti','September','Oktober','November','December'],
            monthNamesShort: ['Jan','Feb','Mar','Apr','Maj','Jun',
            'Jul','Aug','Sep','Okt','Nov','Dec'],
    		dayNamesShort: ['Sön','Mån','Tis','Ons','Tor','Fre','Lör'],
    		dayNames: ['Söndag','Måndag','Tisdag','Onsdag','Torsdag','Fredag','Lördag'],
    		dayNamesMin: ['Sö','Må','Ti','On','To','Fr','Lö'],
            dateFormat: 'yy-mm-dd',
            firstDay: 1,
    		isRTL: false};
    $.datepicker.setDefaults($.datepicker.regional['sv']);

    // Preload images
    var images = ['alerticon.png','callicon.png','lisa_alerstam.jpg','mikael_ohlson.jpg',
                  'reporticon.png','sendicon.png','showicon.png','showreportsicon.png',
                  'smsicon.png','teamicon.png','ulf_jonsson.jpg'];
    for(var i = 0; i < images.length; i++) {
    	var path = '/' + Drupal.settings.tzmobile.base_path + '/images/' + images[i];
    	tzmobile.images[i] = new Image();
    	tzmobile.images[i].src = path;
    }
};

function tzmobile_load(path) {
	path = decodeURIComponent(path);
	var page = eval(path);
	tzmobile.frame.empty().append(page.data);

	// Print history
	tzmobile_update_history(path, page.title);

	// Run callback
	if(page.callback != null) {
		page.callback();
	}

	// TODO: Faster clicking on iPhone by using touchstart, touchmove and touchend
	// see http://cubiq.org/remove-onclick-delay-on-webkit-for-iphone/9

	rebind_links($('body'));
}

function rebind_links(container) {
	container.find('a.tzmobile').bind('click', function (event) {
		event.preventDefault();
		var link = $(this);
		tzmobile_load(link.attr('href').substring(1));
		return false;
	});
}

function tzmobile_update_history(path, title) {
	// find part in history
	var i;
	for(i = 0; i < tzmobile.history.length; ++i) {
		if(tzmobile.history[i].path == path) {
			break;
		}
	}
	tzmobile.history = tzmobile.history.slice(0, i);
	tzmobile.history.push({"path": path, "title": title});

	// Update topbar
	var topbar = '<h1 class="ui-title">' + title + '</h1>';
	var len = tzmobile.history.length;

	if(len > 1) {
		/* If more than two history items, show cancel button in right corner
		 * and the previous page in the left corner */
		topbar += '<a class="tzmobile ui-btn-left ui-btn-left ui-btn ui-btn-icon-left ui-btn-corner-all ui-shadow ui-btn-up-a" data-icon="delete" href="#' +
			tzmobile.history[len - 2].path + '">' +
			'<span class="ui-btn-inner ui-btn-corner-all" aria-hidden="true">' +
			'<span class="ui-btn-text">' +
			tzmobile.history[len - 2].title +
			'</span><span class="ui-icon ui-icon-arrow-l ui-icon-shadow"></span></span>' +
			'</a>';

		/* Also hide the logo when we have left the main menu */
		$('#site-logo').slideUp();
	}
	else {
		/* Show logo again */
		$('#site-logo').slideDown();
	}

	// display
	$('#topbar').html(topbar);
}

function tzmobile_main_menu() {
	var items =
		[
		 {
			 'path': 'tzmobile_updates()',
			 'name': Drupal.t('Updates'),
			 'comment': Drupal.t('Live updates from the team'),
			 'image': 'reporticon.png'
		 },
		 /*{
			 'path': 'tzmobile_report()',
			 'name': Drupal.t('Assignment'),
			 'comment': Drupal.t('Create new assignment'),
			 'image': 'reporticon.png'
		 },*/
		 {
			 'path': 'tzmobile_myteam()',
			 'name': Drupal.t('My team'),
			 'comment': Drupal.t('View team status'),
			 'image': 'teamicon.png'
		 },
/*		 {
			 'path': 'tzmobile_show()',
			 'name': Drupal.t('Show'),
			 'comment': Drupal.t('View time reports'),
			 'image': 'showicon.png'
		 }, */
		 ];

	return {
		'data': Drupal.theme('menu', items),
		'title': Drupal.t('Home')
	};

}





function tzmobile_report() {
    // Load default report data
	tzmobile.report.date = new Date();
	tzmobile.report.begintime = "8:00";
	tzmobile.report.endtime = "17:00";
	tzmobile.report.breakduration = "1:00";

	// Select job
	return tzmobile_report_select_job(0);
}

function tzmobile_report_select_job(id) {
	tzmobile.report.job = id;
	if(tzmobile_report_hassubjob(tzmobile.jobs, id)) {
		return {
			data: Drupal.theme('jobs', tzmobile.jobs, id),
			title: (id != 0) ? tzmobile.jobs[id].name : Drupal.t('Assignment')
		};
	}
	return tzmobile_report_date();
}

function tzmobile_report_date() {
	var content = '<ul class="pageitem"><div id="datepicker" class="widgetbox"></div>' +
		Drupal.theme('menuitem', {path: 'tzmobile_report_begintime()', name: Drupal.t('Next')}) +
		'</ul>';
	return {
		data: content,
		title: Drupal.t('Date'),
		callback: function() {
			$('#datepicker').datepicker({
				defaultDate: tzmobile.report.date,
				maxDate: '+0d',
				onSelect: function(dateText, instance) {
					tzmobile.report.date = $('#datepicker').datepicker("getDate");
					tzmobile_load('tzmobile_report_begintime()');
				}
			});
		}
	};
}


function tzmobile_report_begintime() {
	var content = '<ul class="pageitem"><div id="begintime" class="widgetbox"></div></li></ul>';
	return {
		data: content,
		title: Drupal.t('Start'),
		callback: tzmobile_timefield_callback('begintime', Drupal.t('Start'), 'tzmobile_report_endtime()')
	};
}

function tzmobile_report_endtime() {
	var content = '<ul class="pageitem"><div id="endtime" class="widgetbox"></div></li></ul>';
	return {
		data: content,
		title: Drupal.t('End'),
		callback: tzmobile_timefield_callback('endtime', Drupal.t('End'), 'tzmobile_report_breakduration()')
	};
}

function tzmobile_report_breakduration() {
	var content = '<ul class="pageitem"><div id="breakduration" class="widgetbox"></div></li></ul>';
	return {
		data: content,
		title: Drupal.t('Break'),
		callback: tzmobile_timefield_callback('breakduration', Drupal.t('Break duration'), 'tzmobile_report_summary()', {hideTm: true})
	};
}

function tzmobile_report_summary() {
	var content = '<ul class="pageitem">';
	content += Drupal.theme('report', tzmobile.report);

	var submit = {
			path: tzmobile.history[0].path,
			name: Drupal.t('Submit'),
			image: 'sendicon.png'
	};
	content += Drupal.theme('menuitem', submit);

	content += '</ul>';

	return {
		data: content,
		title: Drupal.t('Summary')
	}
}

function tzmobile_report_hassubjob(jobs, parent) {
	var subjobs = {};
	for(var id in jobs) {
		if(jobs[id].parent == parent) {
			return true;
		}
	}
	return false;
}

function tzmobile_timefield_callback(id, title, nextpath, extra_options) {
	var time = tzmobile.report[id].split(':');
	var opt = {
		defaultHour: time[0],
		defaultMinute: time[1],
		titleLabel: title,
		hoursLabel: Drupal.t('Hour'),
		minutesLabel: Drupal.t('Minute'),
		setButtonLabel: Drupal.t('Next'),
		onSetTime: function(timeText, instance) {
			tzmobile.report[id] = timeText;
			tzmobile_load(nextpath);
		}
	}

	if(extra_options != null) {
		$.extend(opt, extra_options);
	}

	return function() {
		$('#' + id).ptTimeSelect(opt);
	};
}

function tzmobile_time_in_seconds(timestr) {
	var time = 0;
	var fields = timestr.split(':');
	var factor = 3600;
	for(var i = 0; i < fields.length; ++i) {
		time += parseInt(fields[i],10) * factor;
		factor /= 60;
	}
	return time;
}

function tzmobile_updates() {
	var id = "tzmobile-updates-content",
		page = {
			data: $('<div></div>'),
			title: Drupal.t('Updates')
		};
	load_updates(page.data);
	return page;
}

function load_updates(container) {
	var uid = Drupal.settings.tzmobile.uid;
	$.getJSON('/api/updates/' + uid, function(data, status, jqXHR) {
		container.append(Drupal.theme('updates', data));
		rebind_links(container);
	});
}

function tzmobile_myteam() {
	var page = {
		data: $('<div></div>'),
		title: Drupal.t('My team')
	};
	append_team_members(page.data);
	return page;
}

function append_team_members(container) {
	var uid = Drupal.settings.tzmobile.uid;
	$.getJSON('/api/users/?status[-10]=-10&status[0]=0&status[10]=10&status[20]=20&manager=' + uid, function(data, status, jqXHR) {
		container.append(Drupal.theme('teamlist', data));
		rebind_links(container);
	});
}

function tzmobile_get_user(username) {
	var i = 0;
	for(; i < tzmobile.team.length; i++) {
		if(tzmobile.team[i].username == username) break;
	}

	if(i < tzmobile.team.length) {
		return tzmobile.team[i];
	}
	return null;
}

function tzmobile_show_teammember(uid) {
	var page = {
		data: $('<div></div>'),
		title: Drupal.t('User')
	};
	$.getJSON('/api/users/' + uid, function(data, status, jqXHR) {
		page.data.append(Drupal.theme('teammember', data[0]));
		rebind_links(page.data);
		$('#topbar h1').text(data[0].fullname);
	});
	return page;
}

function tzmobile_show_reports(username) {
	var user = tzmobile_get_user(username);
	return {
		data: '',
		title: user.realname
	};
}

function tzmobile_send_reminder(username) {
	$('#reminder .name').text(Drupal.t('Sending reminder...'));
	$('#reminder').unbind('click');
	var onSuccess = function(data) {
		if(data.success) {
			$('#reminder .name').text(Drupal.t('Send reminder'));
			var user = tzmobile_get_user(data.username);
			user.reminder = new Date();
			$('#reminder .comment').text(Drupal.theme('reminder', user.reminder));
		}
	};
	$.ajax({
		url: 'tzmobile/reminder/' + username,
		success: onSuccess,
		dataType: 'json'
	});
}

Drupal.theme.prototype.reminder = function(date) {
	if(date) {
		return Drupal.t('Sent') + ' ' + date.getHours() + ':' + date.getMinutes();
	}
	return '';
};

Drupal.theme.prototype.avatar = function (user) {
	if (user.avatar) {
		return user.avatar;
	}
	return Drupal.settings.tzmobile.base_path + '/images/icon_no_photo_no_border_50x50.png';
}

Drupal.theme.prototype.teamlist = function(teamlist) {
	var content = '<ul data-role="listview" class="ui-listview">';
	for (var i = 0; i < teamlist.length; i++) {
		content += Drupal.theme('teamitem', teamlist[i]);
	}
	content += '</ul>';
	return content;
};

Drupal.theme.prototype.teamitem = function(item) {
	return '<li data-theme="c" class="ui-btn ui-btn-icon-right ui-li-has-arrow ui-li ui-li-has-thumb ui-btn-up-c"><div class="ui-btn-inner ui-li" aria-hidden="true">' +
		'<div class="ui-btn-text">' +
			'<a href="#tzmobile_show_teammember(' + item.id + ')" class="tzmobile ui-link-inherit">' +
				'<img src="' + Drupal.theme('avatar', item) + '" class="avatar ui-li-thumb">' +
				'<h3 class="ui-li-heading">' + item.fullname + '</h3>' +
				'<p class="ui-li-desc">' + (item.due_count > 0 ?
					(Drupal.t('Late time reports:') + ' ' + item.due_count) :
					Drupal.t('All time reported')) + '</p>' +
			'</a>' +
		'</div>' +
		'<span class="ui-icon ui-icon-arrow-r ui-icon-shadow"></span></div></li>';
};

Drupal.theme.prototype.teammember = function(member) {
	var value = '<ul class="ui-listview">';

	// Header
	value += '<li data-theme="c" class="ui-btn ui-li ui-li-has-thumb ui-btn-up-c">' +
		'<div class="ui-btn-inner ui-li" aria-hidden="true">' +
		'<div class="ui-btn-text">';
	value += '<a class="ui-link-inherit"><img src="' + Drupal.theme('avatar', member) + '" class="avatar ui-li-thumb"/>';
	value += '<h3 class="ui-li-heading">' + member.fullname + '</h3>';
	value += '<p class="ui-li-desc">';
	if(member.today != 0) {
		value += Drupal.t('Today') + ': ' + Drupal.theme('longtime', member.today) + '<br/>';
	} else {
		value += '<span style="color: red">' + Drupal.t('No time report today') + '<br/></span>';
	}
	value += Drupal.t('Week') + ': ' + Drupal.theme('longtime', member.week) + '<br/>' +
		Drupal.t('Month') + ': ' + Drupal.theme('longtime', member.month) +
		'</p></a>';
	value += '</div></div></li>';

	// Reminder
	if(member.today == 0) {
		value += '<li class="menu"><a id="reminder" class="noeffect" onclick="tzmobile_send_reminder(\'' + member.username +
		'\')"><img src="' + Drupal.settings.tzmobile.base_path + '/images/alerticon.png"/><span class="name">' +
		Drupal.t('Send reminder') + '</span><span class="comment">' +
		Drupal.theme('reminder', member.reminder) + '</span><span class="arrow"></span></a></li>';
	}

	// Links
	value += '<li data-theme="c" class="ui-btn ui-li ui-li-has-arrow ui-btn-icon-right ui-li-has-thumb ui-btn-up-c">' +
		'<div class="ui-btn-inner ui-li" aria-hidden="true">' +
		'<div class="ui-btn-text">' +
		'<a class="ui-link-inherit" href="tel:' + member.mobile + '">' +
		'<img src="' + Drupal.settings.tzmobile.base_path + '/images/callicon.png" class="ui-li-icon"/>' +
		'<h3 class="ui-li-heading">' + Drupal.t('Call mobile') + '</h3>' +
		'<p class="ui-li-desc">' + member.mobile + '</p>' +
		'</a></div>' +
		'<span class="ui-icon ui-icon-arrow-r ui-icon-shadow"></span></div></li>' +

		'<li data-theme="c" class="ui-btn ui-li ui-li-has-arrow ui-btn-icon-right ui-li-has-thumb ui-btn-up-c">' +
		'<div class="ui-btn-inner ui-li" aria-hidden="true">' +
		'<div class="ui-btn-text">' +
		'<a class="ui-link-inherit" href="sms:' + member.mobile + '">' +
		'<img src="' + Drupal.settings.tzmobile.base_path + '/images/smsicon.png" class="ui-li-icon"/>' +
		'<h3 class="ui-li-heading">' + Drupal.t('Send SMS') + '</h3>' +
		'<p class="ui-li-desc">' + member.mobile + '</p>' +
		'</a></div>' +
		'<span class="ui-icon ui-icon-arrow-r ui-icon-shadow"></span></div></li>' +

		'<li data-theme="c" class="ui-btn ui-li ui-li-has-arrow ui-btn-icon-right ui-li-has-thumb ui-btn-up-c">' +
		'<div class="ui-btn-inner ui-li" aria-hidden="true">' +
		'<div class="ui-btn-text">' +
		'<a class="ui-link-inherit" href="#tzmobile_show_reports(' + member.id + ')">' +
		'<img src="' + Drupal.settings.tzmobile.base_path + '/images/showreportsicon.png" class="ui-li-icon"/>' +
		'<h3 class="ui-li-heading">' + Drupal.t('Show time reports') + '</h3>' +
		'<p class="ui-li-desc">' + member.mobile + '</p>' +
		'</a></div>' +
		'<span class="ui-icon ui-icon-arrow-r ui-icon-shadow"></span></div></li>';

	value += '</ul>';
	return value;
};

Drupal.theme.prototype.jobs = function(jobs, parent) {
	var value = '<span class="graytitle">';
	if(jobs[parent] != null) {
		value += jobs[parent].name;
	} else {
		value += Drupal.t('Assignment');
	}

	var items = [];
	for(var id in jobs) {
		if(jobs[id].parent != parent) {
			continue;
		}
		items.push({path: 'tzmobile_report_select_job(' + id + ')',	name: jobs[id].name});
	}
	value += Drupal.theme('menu', items);

	return value;
};

Drupal.theme.prototype.menu = function(items) {
	var content = '<ul class="ui-listview" data-role="listview">';
	for(var i = 0; i < items.length; i++) {
		content += Drupal.theme('menuitem', items[i]);
	}
	content += '</ul>';
	return content;
};

Drupal.theme.prototype.menuitem = function(item) {
	path = encodeURIComponent(item.path);
	var content = '<li class="ui-btn ui-btn-icon-right ui-li-has-arrow ui-li ui-li-has-thumb ui-btn-up-c">' +
		'<div class="ui-btn-inner ui-li" aria-hidden="true">' +
		'<div class="ui-btn-text">' +
		'<a class="tzmobile ui-link-inherit" title="' + item.name + '" href="#' + path + '">';
	if (item.image != null) {
		content += '<img class="ui-li-icon ui-li-thumb" src="' +
			Drupal.settings.tzmobile.base_path + '/images/' + item.image + '"/>';
	}
	content += '<h3 class="ui-li-heading">' + item.name + '</h3>';
	if(item.comment != null) {
		content += '<p class="ui-li-desc">' + item.comment + '</p>';
	}
	content += '<span class="ui-icon ui-icon-arrow-r ui-icon-shadow"></span>';
	content += '<span class="arrow"></span></a></div></li>';
	return content;
};

Drupal.theme.prototype.longtime = function(seconds) {
	var hours = Math.floor(seconds/3600);
	var minutes = (seconds - hours*3600)/60;
	return hours + ' ' + Drupal.t('hours') + ' ' + minutes + ' ' + Drupal.t('minutes');
};

Drupal.theme.prototype.report = function(report) {
	var content = '<li class="textbox"><span class="header">' +
		Drupal.t('Assignment') + '</span><div><table width="100%" border="0"><tr/>' +
		'<td><strong>Jobb:</strong><td style="text-align: right">';

	// Combine job string
	var jobstr = '';
	var job = report.job;
	while(job != 0) {
		jobstr = tzmobile.jobs[job].name + (jobstr != '' ? ', ' + jobstr : '');
		job = tzmobile.jobs[job].parent;
	}

	// Calculate total work time
	var beginseconds = tzmobile_time_in_seconds(report.begintime);
	var endseconds = tzmobile_time_in_seconds(report.endtime);
	if(endseconds < beginseconds) {
		endseconds += 24*3600;
	}
	var duration = endseconds - beginseconds - tzmobile_time_in_seconds(report.breakduration);

	content += jobstr + '</td><tr/><td><strong>Datum:</strong></td><td style="text-align: right">' +
		$.datepicker.formatDate('yy-mm-dd', report.date) + '</td><tr/><td><strong>Från:</strong></td><td style="text-align: right">' +
		report.begintime + '</td><tr/><td><strong>Till:</strong></td><td style="text-align: right">' +
		report.endtime + '</td><tr/><td><strong>Rastlängd:</strong></td><td style="text-align: right">' +
		report.breakduration + '</td><tr/><td><strong>Totalt:</strong></td><td style="text-align: right">' +
		Drupal.theme('longtime', duration) + '</td>';
	content += '</table></div></li>';

	return content;
};

Drupal.theme.prototype.updates = function(updates) {
	var content = '<ul data-role="listview" class="ui-listview">';
	for (var i = 0; i < updates.length; i++) {
		content += Drupal.theme('update', updates[i]);
	}
	return content;
}

Drupal.theme.prototype.update = function(update) {
	return '<li data-theme="c" class="ui-btn ui-btn-icon-right ui-li ui-li-has-thumb ui-btn-up-c"><div class="ui-btn-inner ui-li" aria-hidden="true">' +
		'<div class="ui-btn-text">' +
			'<a href="#tzmobile_show_teammember(' + update.user.id + ')" class="tzmobile ui-link-inherit">' +
				'<img src="' + Drupal.theme('avatar', update.user) + '" class="avatar ui-li-thumb">' +
				'<h3 class="ui-li-heading">' + update.user.fullname + '</h3>' +
				'<p class="ui-li-desc">' + update.text + '</p>' +
			'</a>' +
		'</div>' +
		'</div></li>';
}
