Chunk-Weather
=============

Weather watch face for Pebble.

This is mostly hacked together from the hard work of many others including: @Katharine, Niknam Moslehi, Martin Rosinski, Hank, @lkorth and probably others.

Known issues
-------------

Android users seem to be missing the weather icon image, but I have no idea why. Please help!


characterRegex
-------------

I had to the patch the Pebble SDK to allow &deg; in the characterRegex. 

"characterRegex": "[:0-9A-Z&deg;]",

[https://github.com/fisakov/pebble-sdk-characterRegex](View Patch)

I think this might be done differently on CloudPebble.

"characterRegex": "[:0-9A-Z\u00b0]",  << or something like that


How to build
-------------

You can build lots of variants of Chunk:

1. Black on White

    resources.json - make sure all image paths are 'images/'
    
        config.h
        
        SPLIT_MODE 0
        
        COLOUR_TEXT_TOP_SPLIT GColorBlack
        
        COLOUR_TEXT_BOTTOM_SPLIT GColorBlack
     
2. White on Black

    resources.json - make sure all image paths are 'black_images/'
    
        config.h
        
        SPLIT_MODE 0
        
        COLOUR_TEXT_TOP_SPLIT GColorWhite
        
        COLOUR_TEXT_BOTTOM_SPLIT GColorWhite
     
3. Split 1 - Black top / White bottom

    resources.json - make sure all image paths are 'black_images/'
    
        config.h
        
        SPLIT_MODE 1
        
        COLOUR_TEXT_TOP_SPLIT GColorWhite
        
        COLOUR_TEXT_BOTTOM_SPLIT GColorBlack
     
4. Split 2 - White top / Black bottom

    resources.json - make sure all image paths are 'images/'
    
        config.h
        
        SPLIT_MODE 1
        
        COLOUR_TEXT_TOP_SPLIT GColorBlack
        
        COLOUR_TEXT_BOTTOM_SPLIT GColorWhite
