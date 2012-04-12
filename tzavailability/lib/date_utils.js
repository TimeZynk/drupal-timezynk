/**
 * Adds extra utility functions to the date object
 */
(function(prototype) {
    /**
     * Returns the ISO 8601 week number for this date
     *
     * @return int
     */
    prototype.getWeek = function() {
        /*
         * getWeek() was developed by Nick Baicoianu at MeanFreePath:
         * http://www.meanfreepath.com
         */

        var dowOffset = 1, // use Monday as first day of week (ISO 8601)
            newYear = new Date(this.getFullYear(), 0, 1),
            day = newYear.getDay() - dowOffset, // the day of week the year begins on
            daynum = Math.floor((this.getTime() - newYear.getTime() -
                        (this.getTimezoneOffset() - newYear.getTimezoneOffset()) * 60000) / 86400000) + 1,
            weeknum;

        day = (day >= 0 ? day : day + 7);
        // if the year starts before the middle of a week
        if (day < 4) {
            weeknum = Math.floor((daynum + day - 1) / 7) + 1;
            if (weeknum > 52) {
                nYear = new Date(this.getFullYear() + 1, 0, 1);
                nday = nYear.getDay() - dowOffset;
                nday = nday >= 0 ? nday : nday + 7;
                /*
                 * if the next year starts before the middle of the week, it is week
                 * #1 of that year
                 */
                weeknum = nday < 4 ? 1 : 53;
            }
        } else {
            weeknum = Math.floor((daynum + day - 1) / 7);
        }
        return weeknum;
    };

    prototype.getISODateString = function(separator) {
        var str = String(this.getFullYear()),
            m = this.getMonth() + 1,
            d = this.getDate();

        if(separator !== undefined) {
            str += separator;
        }

        if (m < 10){str += '0';}
        str += m;

        if(separator !== undefined) {
            str += separator;
        }

        if (d < 10){str += '0';}
        str += d;

        return str;
    };

    prototype.getTimeString = function() {
        var str = '',
            h = this.getHours(),
            m = this.getMinutes();

        if (h < 10){str += '0';}
        str += h + ':';

        if (m < 10){str += '0';}
        str += m;
        return str;
    };

    /**
     * Return the day of week according to the ISO standard. Will use 0 for monday
     * and 6 for sunday.
     */
    prototype.getISODay = function() {
        return (this.getDay() + 6) % 7;
    };

    prototype.getShortDayString = function() {
        return this.getDate() + ' ' + _.month(this.getMonth(), 3);
    };

    prototype.getDayString = function() {
        return _.weekday(this.getDay()) + ', ' + this.getDate() + ' ' + _.month(this.getMonth());
    };

    prototype.getWeekString = function() {
        return _('Week') + ' ' + this.getWeek();
    };

    prototype.getWeekHash = function() {
        var monday = (new Date(this)).addDays(-this.getISODay());
        return 'W' + monday.getISODateString();
    };

    prototype.getShortMonthString = function() {
        return _.month(this.getMonth(), 3);
    };

    prototype.getMonthString = function() {
        return _.month(this.getMonth());
    };

    prototype.getTotalDaysInMonth = function() {
        return 32 - new Date(this.getFullYear(), this.getMonth(), 32).getDate();
    };

    prototype.parseISODate = function(str) {
        var year = str.substr(0, 4),
            month = str.substr(4, 2),
            dayOfMonth = str.substr(6, 2);

        // Parse into integers
        year = parseInt(year, 10);
        month = parseInt(month, 10) - 1;
        dayOfMonth = parseInt(dayOfMonth, 10);

        this.setFullYear(year, month, dayOfMonth);
        return this;
    };

    prototype.getEpoch = function() {
        return Math.floor(this.getTime() / 1000);
    };
    
    prototype.addSeconds = function(seconds) {
        this.setSeconds(this.getSeconds() + seconds, this.getMilliseconds());
        return this;
    };

    prototype.addDays = function(days) {
        this.setDate(this.getDate() + days);
        return this;
    };

    prototype.toStartOfDay = function() {
        this.setHours(0,0,0,0);
        return this;
    };

    prototype.firstDayOfWeek = function() {
        var day = new Date(this);
        // Subtract current weekday to get to monday
        day.addDays(-day.getISODay());
        return day;
    }


})(Date.prototype);

/**
 * Prints a duration in HH:MM from a timestamp in seconds
 */
function print_duration(timestamp) {
    var result = '',
        hours,
        minutes;

    // Change to minutes
    timestamp = Math.floor(timestamp / 60);
    hours = Math.floor(timestamp / 60);
    if (hours < 10) {
        result += '0';
    }
    result += hours + ':';

    minutes = timestamp - hours * 60;
    if (minutes < 10) {
        result += '0';
    }
    result += minutes;

    return result;
}

/**
 * return a duration as HH h MM min
 * @param timestamp timestamp to print
 * @return duration as HH h MM min
 */
function print_duration_long(timestamp) {
    var result = '',
        hours,
        minutes;

    // Change to minutes
    timestamp = Math.floor(timestamp / 60);
    hours = Math.floor(timestamp / 60);
    if(hours > 0) {
        result += hours + ' h ';
    }

    minutes = timestamp - hours * 60;
    result += minutes + ' min';

    return result;
}