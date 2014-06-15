var compareBlogEntry = function(a, b) {
    return b.date.valueOf() - a.date.valueOf();
}

var showBlog = function() {
    $('#content').empty();
    $.each(blogList, function(i, entry) {
        $.ajax({
            type: 'GET',
            url: entry.file,
            dataType: 'text',
            success: function (content) {
                $('#content').append(content);
                $('#content .entry-date').last().text(monthName[entry.date.getMonth()] + ' ' + entry.date.getDate() + ', ' + entry.date.getFullYear());
            }
        });
    });
}

var showPage = function(fileName) {
    $.ajax({
        type: 'GET',
        url: fileName,
        dataType: 'text',
        success: function (content) {
            $('#content').html(content);
        }
    });
}

var selectPage = function(hash) {
    switch (hash) {
        case '#download':
            showPage('download.html');
            break;
        case '#screenshots':
            showPage('screenshots.html');
            break;
        case '#about':
            showPage('about.html');
            break;
        case '#contact':
            showPage('contact.html');
            break;
        default:
            showBlog();
            break;
    }

    var button = $('#menu-box a[href=' + hash + ']').parent();
    button.siblings().removeClass('active');
    button.addClass('active');
}

$('#menu-box a').click( function(event) {
    var button = $(this).parent();
    button.siblings().removeClass('active');
    button.addClass('active');

    $('#content').empty();
    selectPage( $(this).attr('href') );
});

$().ready( function() {
    $.pathchange.init();

    blogList.sort(compareBlogEntry);

    $('#content').empty();
    selectPage(window.location.hash);
});

$(window).bind('pathchange', function() {
    var hash = window.location.hash;
    if (hash.length < 2)
        hash = '#home';

    $('#content').empty();
    selectPage(hash);
});
