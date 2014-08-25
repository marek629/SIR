var compareBlogEntry = function(a, b) {
    return b.date.valueOf() - a.date.valueOf();
};

var showBlog = function() {
    $('#content').empty();
    $.each(blogList, function(i, entry) {
        $.ajax({
            type: 'GET',
            async: false,
            url: entry.file,
            dataType: 'text',
            success: function (content) {
                $('#content').append(content);
                $('#content .entry-date').last().text(
                    monthName[entry.date.getMonth()] + ' ' + entry.date.getDate() + ', ' + entry.date.getFullYear());
            }
        });
    });
};

$().ready( function() {
//    $.pathchange.init();

    blogList.sort(compareBlogEntry);

    $('#content').empty();
    showBlog();
});

// $(window).bind('pathchange', function() {
//     var hash = window.location.hash;
//     if (hash.length < 2)
//         hash = '#home';
//
//     $('#content').empty();
//     selectPage(hash);
// });
