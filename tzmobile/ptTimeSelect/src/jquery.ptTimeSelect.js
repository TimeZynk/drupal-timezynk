/***********************************************************************
 * FILE: jquery.ptTimeSelect.js
 * 
 * 		jQuery plug in for displaying a popup that allows a user
 * 		to define a time and set that time back to a form's input
 * 		field.
 * 
 * 
 * AUTHOR:
 * 
 * 		*Paul T.*
 * 
 * 		- <http://www.purtuga.com>
 * 		- <http://pttimeselect.sourceforge.net>
 * 
 * 
 * DEPENDECIES:
 * 
 * 		-	jQuery.js
 * 			<http://docs.jquery.com/Downloading_jQuery>
 * 
 * 
 * LICENSE:
 * 
 * 		Copyright (c) 2007 Paul T. (purtuga.com)
 *		Dual licensed under the:
 *
 * 		-	MIT
 * 			<http://www.opensource.org/licenses/mit-license.php>
 * 
 * 		-	GPL
 * 			<http://www.opensource.org/licenses/gpl-license.php>
 * 
 * INSTALLATION:
 * 
 * There are two files (.css and .js) delivered with this plugin and
 * that must be incluced in your html page after the jquery.js library,
 * and prior to making any attempts at using it. Both of these are to
 * be included inside of the 'head' element of the document.
 * |
 * |	<link rel="stylesheet" type="text/css" href="jquery.ptTimeSelect.css" />
 * |	<script type="text/javascript" src="jquery.ptTimeSelect.js"></script>
 * |
 * 
 * USAGE:
 * 
 * 	-	See <$(ele).ptTimeSelect()>
 * 
 * 
 * 
 * LAST UPDATED:
 * 
 * 		- $Date: 2009/06/08 22:26:53 $
 * 		- $Author: paulinho4u $
 * 		- $Revision: 1.4 $
 * 
 * 
 **********************************************************************/

jQuery.ptTimeSelect = {};

/***********************************************************************
 * PROPERTY: jQuery.ptTimeSelect.options
 * 		The default options for all timeselect attached elements. Can be
 * 		overwriten wiht <jQuery.fn.ptTimeSelect()>
 * 
 * 	containerClass	-	
 * 
 * 
 */
jQuery.ptTimeSelect.options = {
		containerClass: undefined,
		containerWidth: undefined,
		titleLabel: '',
		hoursLabel: 'Hour',
		minutesLabel: 'Minutes',
		setButtonLabel: 'Set',
		defaultHour: '1',
		defaultMinute: '00',
		hideTm: false,
		popupImage: undefined,
		onFocusDisplay: true,
		onSetTime: undefined
};


/***********************************************************************
 * METHOD: jQuery.ptTimeSelect._ptTimeSelectInit()
 * 		Internal method. Called when page is initalized to add the time
 * 		selection area to the DOM.
 * 
 * PARAMS:
 * 
 * 		none.
 * 
 * RETURNS:
 * 
 * 		nothing.
 * 
 * 
 */
jQuery.ptTimeSelect._addToElem = function (parent) {
	//if the html is not yet created in the document, then do it now
	if (!jQuery('#ptTimeSelectCntr').length) {
		jQuery(parent).append(
				'<div id="ptTimeSelectCntr" class="">'
				+	'		<div class="ui-widget ui-widget-content ui-corner-all">'
				+	'		<div class="ui-widget-header ui-corner-all">'
				+	'			<div id="ptTimeSelectUserTime" style="float: left;">'
				+	'				<span id="ptTimeSelectUserTitle"></span>'
				+	'				<span id="ptTimeSelectUserSelHr">1</span> :'
				+	'				<span id="ptTimeSelectUserSelMin">00</span>'
				+	'			</div>'
				+	'			<br style="clear: both;" /><div></div>'
				+	'		</div>'
				+	'		<div class="ui-widget-content ui-corner-all">'
				+	'			<div>'
				+	'				<div class="ptTimeSelectTimeLabelsCntr">'
				+	'					<div class="ptTimeSelectLeftPane" style="width: 50%; text-align: center; float: left;" class="">Hour</div>'
				+	'					<div class="ptTimeSelectRightPane" style="width: 50%; text-align: center; float: left;">Minutes</div>'
				+	'				</div>'
				+	'				<div>'
				+	'					<div style="float: left; width: 50%;">'
				+	'						<div class="ui-widget-content ptTimeSelectLeftPane">'
				+	'							<div id="ptTimeSelectHrAmPmCntr">'
				+	'								<a id="ptTimeSelectHrAm" class="ui-state-default" onclick="jQuery.ptTimeSelect.setAm();" '
				+	'										style="display: block; width: 54px; float: left;">AM</a>'
				+	'								<a id="ptTimeSelectHrPm" class="ui-state-default" onclick="jQuery.ptTimeSelect.setPm();" '
				+	'										style="display: block; width: 54px; float: left;">PM</a>'
				+	'								<br style="clear: left;" /><div></div>'
				+	'							</div>'
				+	'							<div class="ptTimeSelectHrCntr">'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">0</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">1</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">2</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">3</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">4</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">5</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">6</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">7</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">8</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">9</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">10</a>'
				+	'								<a class="ptTimeSelectHr ui-state-default" href="javascript: void(0);">11</a>'
				+	'								<br style="clear: left;" /><div></div>'
				+	'							</div>'
				+	'						</div>'
				+	'					</div>'
				+	'					<div style="width: 50%; float: left;">'
				+	'						<div class="ui-widget-content ptTimeSelectRightPane">'
				+	'							<div class="ptTimeSelectMinCntr">'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">00</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">05</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">10</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">15</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">20</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">25</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">30</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">35</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">40</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">45</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">50</a>'
				+	'								<a class="ptTimeSelectMin ui-state-default" href="javascript: void(0);">55</a>'
				+	'								<br style="clear: left;" /><div></div>'
				+	'							</div>'
				+	'						</div>'
				+	'					</div>'
				+	'				</div>'
				+	'			</div>'
				+	'			<div style="clear: left;"></div>'
				+	'		</div>'
				+	'	</div>'
				+	'</div>'
				+	'<li class="menu" id="ptTimeSelectSetButton">'
				+	'	<a href="#" id="ptTimeSelectSetLink">'
				+	'		<span class="name">SET</span><span class="arrow"></span>'
				+	'	</a>'
				+	'</li>'
		);

		var e = jQuery('#ptTimeSelectCntr');

		// Add the events to the functions
		e.find('.ptTimeSelectMin')
		.bind("click", function(){
			jQuery.ptTimeSelect.setMin($(this).text());
		});

		e.find('.ptTimeSelectHr')
		.bind("click", function(){
			jQuery.ptTimeSelect.setHr($(this).text());
		});	
		
		jQuery('#ptTimeSelectSetLink').click(function(event) {
			event.preventDefault();
			jQuery.ptTimeSelect.setTime();
		});
	}//end if
}; /* jQuery.ptTimeSelectInit() */


/***********************************************************************
 * METHOD: jQuery.ptTimeSelect.setHr(h)
 * 		Sets the hour selected by the user on the popup.
 * 
 * 
 * PARAMS:
 * 
 * 		h -	[int] interger indicating the hour. This value is the same
 * 			as the text value displayed on the popup under the hour.
 * 			This value can also be the words AM or PM.
 * 
 * 
 * RETURN:
 * 
 * 		none
 */
jQuery.ptTimeSelect.setHr = function(hour) {
	jQuery('#ptTimeSelectUserSelHr').html(hour);
	jQuery('.ptTimeSelectHr').each(function() {
		if(jQuery(this).text() == hour) {
			jQuery(this).addClass('ui-state-highlight ui-state-active');
		} else {
			jQuery(this).removeClass('ui-state-highlight ui-state-active');
		}
	});
};/* END setHr() function */
	
/***********************************************************************
 * METHOD: jQuery.ptTimeSelect.setMin(m)
 * 		Sets the minutes selected by the user on the popup.
 * 
 * 
 * PARAMS:
 * 
 * 		m -	[int] interger indicating the minutes. This value is the same
 * 			as the text value displayed on the popup under the minutes.
 * 
 * 
 * RETURN:
 * 
 * 		none
 */
jQuery.ptTimeSelect.setMin = function(minute) {
	jQuery('#ptTimeSelectUserSelMin').html(minute);
	jQuery('.ptTimeSelectMin').each(function() {
		if(jQuery(this).text() == minute) {
			jQuery(this).addClass('ui-state-highlight ui-state-active');
		} else {
			jQuery(this).removeClass('ui-state-highlight ui-state-active');
		}
	});
};/* END setMin() function */

jQuery.ptTimeSelect.setAm = function() {
	var i = 0;
	jQuery('a.ptTimeSelectHr').each(function() {
		jQuery(this).html(i);
		i++;
	});
	jQuery('#ptTimeSelectHrAm').addClass('ui-state-highlight ui-state-active');
	jQuery('#ptTimeSelectHrPm').removeClass('ui-state-highlight ui-state-active');
	jQuery.ptTimeSelect.setHr(jQuery('#ptTimeSelectUserSelHr').text());
};

jQuery.ptTimeSelect.setPm = function() {
	var i = 12;
	jQuery('a.ptTimeSelectHr').each(function() {
		jQuery(this).html(i);
		i++;
	});
	jQuery('#ptTimeSelectHrAm').removeClass('ui-state-highlight ui-state-active');
	jQuery('#ptTimeSelectHrPm').addClass('ui-state-highlight ui-state-active');
	jQuery.ptTimeSelect.setHr(jQuery('#ptTimeSelectUserSelHr').text());
};

	
/***********************************************************************
 * METHOD: jQuery.ptTimeSelect.setTime()
 * 		Takes the time defined by the user and sets it to the input
 * 		element that the popup is currently opened for.
 * 
 * 
 * PARAMS:
 * 
 * 		none.
 * 
 * 
 * RETURN:
 * 
 * 		none
 */
jQuery.ptTimeSelect.setTime = function() {
	var tSel = jQuery('#ptTimeSelectUserSelHr').text()
				+ ":"
				+ jQuery('#ptTimeSelectUserSelMin').text()
				+ " "
				+ jQuery('#ptTimeSelectUserSelAmPm').text();
	jQuery(".isPtTimeSelectActive").val(tSel);

	var i = $(".isPtTimeSelectActive");
	if (i) {
		var opt = i.data("ptTimeSelectOptions");
		if (opt && opt.onSetTime) {
			opt.onSetTime(tSel, i);
		}
	}
};/* END setTime() function */
	
/***********************************************************************
 * METHOD: jQuery.ptTimeSelect.openCntr()
 * 		Displays the time definition area on the page, right below
 * 		the input field.  Also sets the custom colors/css on the
 * 		displayed area to what ever the input element options were
 * 		set with.
 * 
 * PARAMS:
 * 
 * 		uId	-	STRING. Id of the element for whom the area will
 * 				be displayed. This ID was created when the 
 * 				ptTimeSelect() method was called.
 * 
 * RETURN:
 * 
 * 		nothing.
 * 
 */
jQuery.ptTimeSelect.openCntr = function (ele) {
	jQuery.ptTimeSelect._addToElem(ele);
	jQuery(".isPtTimeSelectActive").removeClass("isPtTimeSelectActive");
	var cntr			= jQuery("#ptTimeSelectCntr");
	var i				= jQuery(ele).eq(0).addClass("isPtTimeSelectActive");
	var opt				= i.data("ptTimeSelectOptions");
	var style			= i.offset();
	if (opt.containerWidth) {
		style.width = opt.containerWidth;
	}
	if (opt.containerClass) {
		cntr.addClass(opt.containerClass);
	}
	cntr.css(style);
	var hr	= opt.defaultHour ? opt.defaultHour : 1;
	var min	= opt.defaultMinute ? opt.defaultMinute : '00';
	
	if(hr < 12) {
		jQuery.ptTimeSelect.setAm();
	} else {
		jQuery.ptTimeSelect.setPm();
	}
	
	if(opt.hideTm) {
		$('#ptTimeSelectHrAmPmCntr').remove();
	}
	
	if(opt.titleLabel) {
		cntr.find("#ptTimeSelectUserTitle").html(opt.titleLabel);
	}

	jQuery.ptTimeSelect.setHr(hr);
	jQuery.ptTimeSelect.setMin(min);

	cntr.find(".ptTimeSelectTimeLabelsCntr .ptTimeSelectLeftPane")
		.empty().append(opt.hoursLabel);
	cntr.find(".ptTimeSelectTimeLabelsCntr .ptTimeSelectRightPane")
		.empty().append(opt.minutesLabel);
	jQuery("#ptTimeSelectSetButton a span.name").empty().append(opt.setButtonLabel);	
};/* END openCntr() function */


jQuery.ptTimeSelect._doCheckMouseClick = function(ev){
	if (!$("#ptTimeSelectCntr:visible").length) {
		return;
	}
	if (!jQuery(ev.target).closest("#ptTimeSelectCntr").length){
		jQuery.ptTimeSelect.closeCntr();
	}
	
};/* jQuery.ptTimeSelect._doCheckMouseClick */


/***********************************************************************
 * METHOD: $(ele).ptTimeSelect()
 * 	Attaches a ptTimeSelect widget to each matched element. Matched
 * 	elements must be input fields that accept a values (input field).
 * 	Each element, when focused upon, will display a time selection 
 * 	popoup where the user can define a time.
 * 
 * PARAMS:
 * 
 * 	@param {OBJECT}	opt -	(Optional) An object with the options for
 * 							the time selection widget.
 * 
 * OPTIONS:
 * 
 * 	containerClass	-	String. A class to be assocated with the popup widget.
 * 						(default: none)
 * 	containerWidth	-	String. Css width for the container. (default: none)
 * 	hoursLabel		-	String. Label for the Hours. (default: Hours)
 * 	minutesLabel	-	String. Label for the Mintues. (default: Minutes)
 * 	setButtonLabel	-	String. Label for the Set button. (default: SET)
 * 	popupImage		-	String. The html element (ex. img or text) to be
 * 						appended next to each input field and that will display
 * 						the time select widget upon click.
 * 
 * 
 * RETURNS:
 * 
 * 		- @return {object} jQuery
 * 
 * 
 * EXAMPLE:
 * 
 * 	|		$('#fooTime').ptTimeSelect();
 * 
 */
jQuery.fn.ptTimeSelect = function (opt) {
	this.each(function(){
		if(this.nodeName.toLowerCase() != 'div') {
			return
		}

		var e = jQuery(this);
		if (e.hasClass('hasPtTimeSelect')){
			return this;
		}
		var thisOpt = {};
		thisOpt = $.extend(thisOpt, jQuery.ptTimeSelect.options, opt);
		e.addClass('hasPtTimeSelect').data("ptTimeSelectOptions", thisOpt);
		
		jQuery.ptTimeSelect.openCntr(this);
		
		return this;
	});
};/* End of jQuery.fn.timeSelect */


/***********************************************************************
 * SECTION: HTML INSETED INTO DOM
 * 	The only html created on the page is the popup window widget. For
 * 	details on the structure of this element see
 * 	<jQuery.ptTimeSelect._ptTimeSelectInit()>
 * 
 */


