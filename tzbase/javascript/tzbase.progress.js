(function($) {
	$.runWithProgressBar = function(employees, options) {
        var args = $.extend({
	        	title: Drupal.t('Sending') + '...',
	        	chunk_size: 1,
	        	on_process: function(elements, on_success) {setTimeout(on_success, 1);},
	        	on_finished: function() {},
	        	format_progress: function(current, total) { 
	        						return Drupal.t('@count of @total', {
						                "@count": current,
						                "@total": total
						            }); 
	        					 },
	        	id: 'tzbase-progress-dialog'
	        }, options),
	        dialog,
            progressbar,
            info;

        $('#' + args.id).remove();
        dialog = $('<div id="' + args.id + '" title="' + args.title + '"></div>');

        progressbar = $('<div></div>');
        dialog.append(progressbar);
        progressbar.progressbar();

        info = $('<div class="messages status"></div>');
        dialog.append(info);

        dialog.dialog({modal: true});

        function processEmployeeChunks() {
            var sent = 0,
                total = employees.length;

            function nextEmployeeChunk() {
                sent += args.chunk_size;
                sent = Math.min(total, sent);
                progressbar.progressbar('value', sent*100/total);
                info.text(args.format_progress(sent, total));

                if (sent < total) {
                    args.on_process(employees.slice(sent, sent + args.chunk_size), nextEmployeeChunk);
                } else {
                    setTimeout(function() {
                        dialog.dialog('close');
                        dialog.remove();
                        args.on_finished();
                    }, 1000);
                }
            }

            // Slice employees into chunks of chunk_size
            args.on_process(employees.slice(sent, sent + args.chunk_size), nextEmployeeChunk);
        }

        processEmployeeChunks();
    };
})(jQuery);