# AsteroidOS Weather Fetch App
This app uses an internet connection from the watch to fetch weather forecast data to be used for the Weather app in [AsteroidOS](http://asteroidos.org/)

## GUI version

## Command-line version
There is also a command-line version called `weatherfetch_cli` which is intended to be used from a command line.  On the watch command line as the `ceres` user, one can run the command with no arguments to fetch the weather forecast for the previously configured location.

If the user would like to temporarily add a new location and fetch weather for that location:

```
weatherfetch_cli -cl --add 50.812375,4.380734,"Université libre de Bruxelles"
```

In the following, TOL means Top-Of-List and BOL means Bottom-Of-List.  These refer to the saved location list.  Here are the possible commands

<table>
<thead>
<tr><th>Short</th><th>Long</th><th>Args</th><th>Description</th></tr>
</thead>
<tbody>
<tr><td>-a</td><td>--add</td><td>lat,lng,name</td><td>add the location to top of list (TOL)</tr>
<tr><td>-c</td><td>--config</td><td></td><td>manipulate the list but don't fetch weather</td></tr>
<tr><td>-d</td><td>--delete</td><td></td><td>delete BOL</td></tr>
<tr><td>-l</td><td>--list</td><td></td><td>list all current locations</td></tr>
<tr><td>-r</td><td>--rotate</td><td></td><td>rotate the list to put TOL at BOL</td></tr>
</tbody>
</table>
These should always execute in the order "rotate", "delete", "add", "list", "config" no matter their order on the command line. The (admittedly weak) reasoning for this order is that one can do simple testing with something like this:

```
weatherfetch_cli -rdlc --add 50.812375,4.380734,"Université libre de Bruxelles"
```

And the effect is that each time it is run, it will rotate the top entry to the bottom, delete it, add the new entry to the top, and then list the result, all without actually fetching the weather (that's the -c) and annoying or abusing that service during testing.
