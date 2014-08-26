var compareBlogEntry = function(a, b) {
    return b.date.valueOf() - a.date.valueOf();
};

var getEntryPage = function(entry) {
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
};

var showBlog = function() {
    $('#content').empty();

    var yearArray = new Array();
    var activeYear = 0;

    var hash = window.location.hash.replace('#', '');
    if (hash.length == 0)
        activeYear = blogList[0].date.getFullYear();
    else {
        var int = parseInt(hash);
        if (isNaN(int))
            activeYear = blogList[0].date.getFullYear();
        else
            activeYear = int;
    }

    $.each(blogList, function(i, entry) {
        var year = entry.date.getFullYear();

        if ($.inArray(year, yearArray) < 0) {
            yearArray.push(year);
            $('#menu-box').append(
                '<li>' +
                    '<a href="#' + year + '">' + year + '</a>' +
                '</li>'
            );
        }

        if (year == activeYear)
            getEntryPage(entry);
    });

    $('#menu-box li:contains("' + activeYear + '")').first().addClass('active');
};

$().ready( function() {
    $.pathchange.init();

    blogList.sort(compareBlogEntry);

    $('#content').empty();
    showBlog();
});

$(window).bind('pathchange', function() {
    $('#content').empty();
    $('#menu-box').empty();
    showBlog();
});
