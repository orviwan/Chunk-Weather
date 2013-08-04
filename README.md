Chunk-Weather
=============

Weather watch face for Pebble.

<a href="http://imgur.com/mSByofK"><img src="http://i.imgur.com/mSByofK.png" title="Hosted by imgur.com"/></a>

This is mostly hacked together from the hard work of many others including: @Katharine, Niknam Moslehi, Martin Rosinski, Hank, @lkorth and probably others.


characterRegex
-------------

I had to the patch the Pebble SDK to allow &deg; in the characterRegex. 

"characterRegex": "[:0-9A-Z&deg;]",

[https://github.com/fisakov/pebble-sdk-characterRegex]

I think this might be done differently on CloudPebble.

"characterRegex": "[:0-9A-Z\u00b0]",  << or something like that

Fonts
-------------

I have excluded the .otf files used from github, but instructions to download them can be found in /resources/src/fonts/fonts.txt

Images
-------------

The weather icons included with this release are copyright 'Mike Afford Media'.

You are free to use this set of icons in any Pebble watch face, but please include this licence with any source distribution.

If you want the full set of icons in much better quality, please visit:

[Buy Now](http://www.mikeafford.com/store/index.html)


How to build
-------------

You can build lots of variants of Chunk, just edit the config file.

1. Black on White 
2. White on Black
3. Split 1 - White top / Black bottom     
4. Split 2 - Black top / White bottom
