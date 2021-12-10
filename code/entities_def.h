#ifndef ENTITIES_DEF_H
#define ENTITIES_DEF_H

enum entities_name
{
    MANUTO,
    HAILEE,
    CAMILA,
    SABRINA,
    THOMEX,
    BIG_DADDY,
    GONZITRON,
    JHOSEP
};

entity EntitiesDef[] = {
    // MANUTO
    {
        2,                  // id
        HERO,               // type
        {-1, -1, -1, -1},   // actions
        0,                  // num of acitions
        "Manuto",           // name
        {
            100,            // hp max
            100,            // hp now
            25,             // mp max
            25,             // mp now
            12,             // strength
            20,             // speed
            5               // intelligence
        },         
        2,                  // weapon index
        {0, -1, -1},        // spells index      
        1,                  // sells count
        0,                  // dialogue
        0,                  // stage
        1,                  // skin
        0,                  // frame
        BIT(DOWN),          // facing
        0,                  // initial layer
        {0.0f, 0.0f},       // position
        {0.0f, 0.0f},       // old position
        {0.0f, 0.0f},       // next position
        0.0f,               // walk distance
        0.0f,               // animation timer
        2.0f,               // time to wait
        0.0f,               // wait time
        false,              // is walking
        true                // alive
    },
    // HAILEE
    {
        3,                  // id
        HERO,               // type
        {-1, -1, -1, -1},   // actions
        0,                  // num of acitions
        "Hailee",           // name
        {
            100,            // hp max
            100,            // hp now
            25,             // mp max
            25,             // mp now
            12,             // strength
            20,             // speed
            5               // intelligence
        },         
        1,                  // weapon index
        {0, -1, -1},        // spells index      
        1,                  // sells count
        0,                  // dialogue
        0,                  // stage
        2,                  // skin
        0,                  // frame
        BIT(DOWN),          // facing
        0,                  // initial layer
        {0.0f, 0.0f},       // position
        {0.0f, 0.0f},       // old position
        {0.0f, 0.0f},       // next position
        0.0f,               // walk distance
        0.0f,               // animation timer
        2.0f,               // time to wait
        0.0f,               // wait time
        false,              // is walking
        true                // alive
    },
    // CAMILA
    {
        4,                  // id
        HERO,               // type
        {-1, -1, -1, -1},   // actions
        0,                  // num of acitions
        "Camila",           // name
        {
            100,            // hp max
            100,            // hp now
            25,             // mp max
            25,             // mp now
            12,             // strength
            20,             // speed
            5               // intelligence
        },         
        0,                  // weapon index
        {0, -1, -1},        // spells index      
        1,                  // sells count
        0,                  // dialogue
        0,                  // stage
        7,                  // skin
        0,                  // frame
        BIT(DOWN),          // facing
        0,                  // initial layer
        {0.0f, 0.0f},       // position
        {0.0f, 0.0f},       // old position
        {0.0f, 0.0f},       // next position
        0.0f,               // walk distance
        0.0f,               // animation timer
        2.0f,               // time to wait
        0.0f,               // wait time
        false,              // is walking
        true                // alive
    },
    // SABRINA
    {
        5,                  // id
        HERO,               // type
        {-1, -1, -1, -1},   // actions
        0,                  // num of acitions
        "Sabrina",          // name
        {
            100,            // hp max
            100,            // hp now
            25,             // mp max
            25,             // mp now
            12,             // strength
            20,             // speed
            5               // intelligence
        },         
        1,                  // weapon index
        {0, -1, -1},        // spells index      
        1,                  // sells count
        0,                  // dialogue
        0,                  // stage
        8,                  // skin
        0,                  // frame
        BIT(DOWN),          // facing
        0,                  // initial layer
        {0.0f, 0.0f},       // position
        {0.0f, 0.0f},       // old position
        {0.0f, 0.0f},       // next position
        0.0f,               // walk distance
        0.0f,               // animation timer
        2.0f,               // time to wait
        0.0f,               // wait time
        false,              // is walking
        true                // alive
    },
    // THOMEX
    {
        6,                  // id
        NPC,                // type
        {-1, -1, -1, -1},   // actions
        0,                  // num of acitions
        "Thomex",           // name
        {
            20,             // hp max
            20,             // hp now
            5,              // mp max
            5,              // mp now
            12,             // strength
            5,              // speed
            5               // intelligence
        },         
        2,                  // weapon index
        {0, -1, -1},        // spells index      
        1,                  // sells count
        1,                  // dialogue
        0,                  // stage
        2,                  // skin
        0,                  // frame
        BIT(DOWN),          // facing
        0,                  // initial layer
        {0.0f, 0.0f},       // position
        {0.0f, 0.0f},       // old position
        {0.0f, 0.0f},       // next position
        0.0f,               // walk distance
        0.0f,               // animation timer
        1.0f,               // time to wait
        0.0f,               // wait time
        false,              // is walking
        true                // alive
    },
    // BIG DADDY
    {
        7,                  // id
        NPC,                // type
        {-1, -1, -1, -1},   // actions
        0,                  // num of acitions
        "Big Daddy",        // name
        {
            30,             // hp max
            30,             // hp now
            5,              // mp max
            5,              // mp now
            12,             // strength
            10,              // speed
            5               // intelligence
        },         
        0,                  // weapon index
        {0, -1, -1},        // spells index      
        1,                  // sells count
        0,                  // dialogue
        0,                  // stage
        3,                  // skin
        0,                  // frame
        BIT(DOWN),          // facing
        0,                  // initial layer
        {0.0f, 0.0f},       // position
        {0.0f, 0.0f},       // old position
        {0.0f, 0.0f},       // next position
        0.0f,               // walk distance
        0.0f,               // animation timer
        3.0f,               // time to wait
        0.0f,               // wait time
        false,              // is walking
        true                // alive
    },
    // GONZITRON
    {
        8,                  // id
        ENEMY,              // type
        {-1, -1, -1, -1},   // actions
        0,                  // num of acitions
        "Gonzitron",        // name
        {
            25,             // hp max
            25,             // hp now
            5,              // mp max
            5,              // mp now
            8,              // strength
            4,              // speed
            12              // intelligence
        },         
        0,                  // weapon index
        {0, -1, -1},        // spells index      
        1,                  // sells count
        0,                  // dialogue
        0,                  // stage
        6,                  // skin
        0,                  // frame
        BIT(RIGHT),         // facing
        0,                  // initial layer
        {0.0f, 0.0f},       // position
        {0.0f, 0.0f},       // old position
        {0.0f, 0.0f},       // next position
        0.0f,               // walk distance
        0.0f,               // animation timer
        -1.0f,              // time to wait
        0.0f,               // wait time
        false,              // is walking
        true                // alive
    },
    // JHOSEP
    {
        9,                  // id
        ENEMY,              // type
        {-1, -1, -1, -1},   // actions
        0,                  // num of acitions
        "Jhosep",           // name
        {
            10,             // hp max
            10,             // hp now
            15,             // mp max
            15,             // mp now
            6,              // strength
            1,              // speed
            8               // intelligence
        },         
        0,                  // weapon index
        {0, -1, -1},        // spells index      
        1,                  // sells count
        0,                  // dialogue
        0,                  // stage
        5,                  // skin
        0,                  // frame
        BIT(RIGHT),         // facing
        0,                  // initial layer
        {0.0f, 0.0f},       // position
        {0.0f, 0.0f},       // old position
        {0.0f, 0.0f},       // next position
        0.0f,               // walk distance
        0.0f,               // animation timer
        -1.0f,              // time to wait
        0.0f,               // wait time
        false,              // is walking
        true                // alive
    }
};

#endif
