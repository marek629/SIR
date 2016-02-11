// Blog Data

var blogList = [
    {   'date': new Date(2016, 1, 11),
        'file': 'release_3.1.html',
        'author': 'marek'
    },
    {   'date': new Date(2016, 0, 6),
        'file': 'release_3.0.html',
        'author': 'marek'
    },
    {   'date': new Date(2015, 2, 5),
        'file': 'release_2.8.html',
        'author': 'marek'
    },
    {   'date': new Date(2015, 0, 6),
        'file': 'video_2.1.1_to_2.5.1.html',
        'author': 'marek'
    },
    {   'date': new Date(2014, 7, 24),
        'file': 'release_2.7.3.html',
        'author': 'marek'
    },
    {   'date': new Date(2014, 4, 31),
        'file': 'release_2.7.2.html',
        'author': 'marek'
    },
    {   'date': new Date(2014, 3, 24),
        'file': 'installer_2.7.1.html',
        'author': 'marek'
    },
    {   'date': new Date(2014, 3, 17),
        'file': 'release_2.7.1.html',
        'author': 'marek'
    },
    {   'date': new Date(2014, 3, 10),
        'file': 'new_site.html',
        'author': 'marek'
    },
    {   'date': new Date(2014, 1, 28),
        'file': 'release_2.7.html',
        'author': 'marek'
    },
    {   'date': new Date(2013, 3, 28),
        'file': 'release_2.6.html',
        'author': 'marek'
    },
    {   'date': new Date(2012, 10, 14),
        'file': 'release_2.5.1.html',
        'author': 'marek'
    },
    {   'date': new Date(2012, 10, 13),
        'file': 'release_2.5.html',
        'author': 'marek'
    }
]

var monthName = [
    'January', 'February', 'March',
    'April', 'May', 'June',
    'July', 'August', 'September',
    'October', 'November', 'December'
];

// Blog Logic

var getEntryPage = function(entry) {
    $.ajax({
        type: 'GET',
        async: false,
        url: entry.file,
        dataType: 'text',
        success: function (content) {
            $('#content').append(content);
            var date = entry.date;
            var timeTag = $('#content .entry-date').last();
            timeTag.text(
                monthName[date.getMonth()] + ' ' + date.getDate() + ', ' + date.getFullYear()
            );
            timeTag.attr(
                "datetime",
                date.toDateString()
            );
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
                    '<a href="#' + year + '">' + year +
                        '<span class="badge pull-right">0</span>' +
                    '</a>' +
                '</li>'
            );
        }

        var yearBadge = $('#menu-box li:contains("' + year + '") span.badge');
        var count = parseInt(yearBadge.text());
//         count += 1;
        yearBadge.text(count + 1);

        if (year == activeYear)
            getEntryPage(entry);
    });

    $('#menu-box li:contains("' + activeYear + '")').first().addClass('active');
};

var compareBlogEntry = function(a, b) {
    return b.date.valueOf() - a.date.valueOf();
};

$().ready( function() {
    $.pathchange.init();

    blogList.sort(compareBlogEntry);

    $('#content').empty();
    showBlog();
    onWindowResize();
});

$(window).resize( onWindowResize );
function onWindowResize() {
    var menuBox = $('#menu-box');
    if ($(window).width() > 768) menuBox.addClass('nav-stacked');
    else menuBox.removeClass('nav-stacked');
}

$(window).bind('pathchange', function() {
    $('#content').empty();
    $('#menu-box').empty();
    showBlog();
});
