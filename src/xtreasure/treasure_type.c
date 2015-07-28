#include "treasure_type.h"

#include <assert.h>
#include <stdlib.h>

#include "common/alloc_or_die.h"
#include "common/dice.h"
#include "common/fail.h"
#include "common/str.h"

#include "gem.h"
#include "jewelry.h"
#include "MagicItem.h"
#include "Treasure.h"
#include "treasure_map.h"


struct coins_gems_or_jewelry {
    char const *amount;
    int percent_chance;
    bool is_per_individual;
};


struct maps_or_magic_type {
    int amount;
    char const *variable_amount;
    bool is_map_possible;
    PossibleMagicItems possible_magic_items;
};


struct maps_or_magic {
    int percent_chance;
    struct maps_or_magic_type types[6];
    int types_count;
};


struct treasure_type {
    char letter;
    struct coins_gems_or_jewelry copper;
    struct coins_gems_or_jewelry silver;
    struct coins_gems_or_jewelry electrum;
    struct coins_gems_or_jewelry gold;
    struct coins_gems_or_jewelry platinum;
    struct coins_gems_or_jewelry gems;
    struct coins_gems_or_jewelry jewelry;
    struct maps_or_magic maps_or_magic;
};


static struct treasure_type treasure_types[] = {
    {
        .letter='A',
        .copper={
            .amount="1d6x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .silver={
            .amount="1d6x1000",
            .percent_chance=30,
            .is_per_individual=false
        },
        .electrum={
            .amount="1d6x1000",
            .percent_chance=35,
            .is_per_individual=false
        },
        .gold={
            .amount="1d10x1000",
            .percent_chance=40,
            .is_per_individual=false
        },
        .platinum={
            .amount="1d4x100",
            .percent_chance=25,
            .is_per_individual=false
        },
        .gems={
            .amount="4d10",
            .percent_chance=60,
            .is_per_individual=false
        },
        .jewelry={
            .amount="3d10",
            .percent_chance=50,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=30,
            .types={
                {
                    .amount=3,
                    .variable_amount="",
                    .is_map_possible=true,
                    .possible_magic_items=ANY_MAGIC_ITEM
                }
            },
            .types_count=1
        }
    },
    {
        .letter='B',
        .copper={
            .amount="1d8x1000",
            .percent_chance=50,
            .is_per_individual=false
        },
        .silver={
            .amount="1d6x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .electrum={
            .amount="1d4x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .gold={
            .amount="1d3x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="1d8",
            .percent_chance=30,
            .is_per_individual=false
        },
        .jewelry={
            .amount="1d4",
            .percent_chance=20,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=10,
            .types={
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=MAGIC_WEAPON_OR_ARMOR
                }
            },
            .types_count=1
        }
    },
    {
        .letter='C',
        .copper={
            .amount="1d12x1000",
            .percent_chance=20,
            .is_per_individual=false
        },
        .silver={
            .amount="1d6x1000",
            .percent_chance=30,
            .is_per_individual=false
        },
        .electrum={
            .amount="1d4x1000",
            .percent_chance=10,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="1d6",
            .percent_chance=25,
            .is_per_individual=false
        },
        .jewelry={
            .amount="1d3",
            .percent_chance=20,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=10,
            .types={
                {
                    .amount=2,
                    .variable_amount="",
                    .is_map_possible=true,
                    .possible_magic_items=ANY_MAGIC_ITEM
                }
            },
            .types_count=1
        }
    },
    {
        .letter='D',
        .copper={
            .amount="1d8x1000",
            .percent_chance=10,
            .is_per_individual=false
        },
        .silver={
            .amount="1d12x1000",
            .percent_chance=15,
            .is_per_individual=false
        },
        .electrum={
            .amount="1d8x1000",
            .percent_chance=15,
            .is_per_individual=false
        },
        .gold={
            .amount="1d6x1000",
            .percent_chance=50,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="1d10",
            .percent_chance=30,
            .is_per_individual=false
        },
        .jewelry={
            .amount="1d6",
            .percent_chance=25,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=15,
            .types={
                {
                    .amount=2,
                    .variable_amount="",
                    .is_map_possible=true,
                    .possible_magic_items=ANY_MAGIC_ITEM
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=POTION
                }
            },
            .types_count=2
        }
    },
    {
        .letter='E',
        .copper={
            .amount="1d10x1000",
            .percent_chance=5,
            .is_per_individual=false
        },
        .silver={
            .amount="1d12x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .electrum={
            .amount="1d6x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .gold={
            .amount="1d8x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="1d12",
            .percent_chance=15,
            .is_per_individual=false
        },
        .jewelry={
            .amount="1d8",
            .percent_chance=10,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=25,
            .types={
                {
                    .amount=3,
                    .variable_amount="",
                    .is_map_possible=true,
                    .possible_magic_items=ANY_MAGIC_ITEM
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=SCROLL
                }
            },
            .types_count=2
        }
    },
    {
        .letter='F',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="1d20x1000",
            .percent_chance=10,
            .is_per_individual=false
        },
        .electrum={
            .amount="1d12x1000",
            .percent_chance=15,
            .is_per_individual=false
        },
        .gold={
            .amount="1d10x1000",
            .percent_chance=40,
            .is_per_individual=false
        },
        .platinum={
            .amount="1d8x100",
            .percent_chance=35,
            .is_per_individual=false
        },
        .gems={
            .amount="3d10",
            .percent_chance=20,
            .is_per_individual=false
        },
        .jewelry={
            .amount="1d10",
            .percent_chance=10,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=30,
            .types={
                {
                    .amount=3,
                    .variable_amount="",
                    .is_map_possible=true,
                    .possible_magic_items=NON_WEAPON_MAGIC
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=POTION
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=SCROLL
                }
            },
            .types_count=3
        }
    },
    {
        .letter='G',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="10d4x1000",
            .percent_chance=50,
            .is_per_individual=false
        },
        .platinum={
            .amount="1d20x100",
            .percent_chance=50,
            .is_per_individual=false
        },
        .gems={
            .amount="5d4",
            .percent_chance=30,
            .is_per_individual=false
        },
        .jewelry={
            .amount="1d10",
            .percent_chance=25,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=35,
            .types={
                {
                    .amount=4,
                    .variable_amount="",
                    .is_map_possible=true,
                    .possible_magic_items=ANY_MAGIC_ITEM
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=SCROLL
                }
            },
            .types_count=2
        }
    },
    {
        .letter='H',
        .copper={
            .amount="5d6x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .silver={
            .amount="1d100x1000",
            .percent_chance=40,
            .is_per_individual=false
        },
        .electrum={
            .amount="10d4x1000",
            .percent_chance=40,
            .is_per_individual=false
        },
        .gold={
            .amount="10d6x1000",
            .percent_chance=55,
            .is_per_individual=false
        },
        .platinum={
            .amount="5d10x100",
            .percent_chance=25,
            .is_per_individual=false
        },
        .gems={
            .amount="1d100",
            .percent_chance=50,
            .is_per_individual=false
        },
        .jewelry={
            .amount="10d4",
            .percent_chance=50,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=15,
            .types={
                {
                    .amount=4,
                    .variable_amount="",
                    .is_map_possible=true,
                    .possible_magic_items=ANY_MAGIC_ITEM
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=POTION
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=SCROLL
                }
            },
            .types_count=3
        }
    },
    {
        .letter='I',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="3d6x100",
            .percent_chance=30,
            .is_per_individual=false
        },
        .gems={
            .amount="2d10",
            .percent_chance=55,
            .is_per_individual=false
        },
        .jewelry={
            .amount="1d12",
            .percent_chance=50,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=15,
            .types={
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=true,
                    .possible_magic_items=ANY_MAGIC_ITEM
                }
            },
            .types_count=1
        }
    },
    {
        .letter='J',
        .copper={
            .amount="3d8",
            .percent_chance=100,
            .is_per_individual=true
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='K',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="3d6",
            .percent_chance=100,
            .is_per_individual=true
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='L',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="2d6",
            .percent_chance=100,
            .is_per_individual=true
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='M',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="2d4",
            .percent_chance=100,
            .is_per_individual=true
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='N',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="1d6",
            .percent_chance=100,
            .is_per_individual=true
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='O',
        .copper={
            .amount="1d4x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .silver={
            .amount="1d3x1000",
            .percent_chance=20,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='P',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="1d6x1000",
            .percent_chance=30,
            .is_per_individual=false
        },
        .electrum={
            .amount="1d2x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='Q',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="1d4",
            .percent_chance=50,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='R',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="2d4x1000",
            .percent_chance=40,
            .is_per_individual=false
        },
        .platinum={
            .amount="10d6x100",
            .percent_chance=50,
            .is_per_individual=false
        },
        .gems={
            .amount="4d8",
            .percent_chance=55,
            .is_per_individual=false
        },
        .jewelry={
            .amount="1d12",
            .percent_chance=45,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='S',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=40,
            .types={
                {
                    .amount=0,
                    .variable_amount="2D4",
                    .is_map_possible=false,
                    .possible_magic_items=POTION
                }
            },
            .types_count=1
        }
    },
    {
        .letter='T',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=50,
            .types={
                {
                    .amount=0,
                    .variable_amount="1D4",
                    .is_map_possible=false,
                    .possible_magic_items=SCROLL
                }
            },
            .types_count=1
        }
    },
    {
        .letter='U',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="10d8",
            .percent_chance=90,
            .is_per_individual=false
        },
        .jewelry={
            .amount="5d6",
            .percent_chance=80,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=70,
            .types={
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=RING
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=ROD_STAFF_WAND
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=MISC_MAGIC
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=ARMOR_SHIELD
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=SWORD
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=MISC_WEAPON
                }
            },
            .types_count=6
        }
    },
    {
        .letter='V',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=85,
            .types={
                {
                    .amount=2,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=RING
                },
                {
                    .amount=2,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=ROD_STAFF_WAND
                },
                {
                    .amount=2,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=MISC_MAGIC
                },
                {
                    .amount=2,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=ARMOR_SHIELD
                },
                {
                    .amount=2,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=SWORD
                },
                {
                    .amount=2,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=MISC_WEAPON
                }
            },
            .types_count=6
        }
    },
    {
        .letter='W',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="5d6x1000",
            .percent_chance=60,
            .is_per_individual=false
        },
        .platinum={
            .amount="1d8x100",
            .percent_chance=15,
            .is_per_individual=false
        },
        .gems={
            .amount="10d8",
            .percent_chance=60,
            .is_per_individual=false
        },
        .jewelry={
            .amount="5d8",
            .percent_chance=50,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=55,
            .types={
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=true,
                    .possible_magic_items=NO_MAGIC_ITEM
                }
            },
            .types_count=1
        }
    },
    {
        .letter='X',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=60,
            .types={
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=MISC_MAGIC
                },
                {
                    .amount=1,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=POTION
                }
            },
            .types_count=2
        }
    },
    {
        .letter='Y',
        .copper={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .silver={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .electrum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gold={
            .amount="2d6x1000",
            .percent_chance=70,
            .is_per_individual=false
        },
        .platinum={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .gems={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .jewelry={
            .amount="",
            .percent_chance=0,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=0,
            .types={},
            .types_count=0
        }
    },
    {
        .letter='Z',
        .copper={
            .amount="1d3x1000",
            .percent_chance=20,
            .is_per_individual=false
        },
        .silver={
            .amount="1d4x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .electrum={
            .amount="1d4x1000",
            .percent_chance=25,
            .is_per_individual=false
        },
        .gold={
            .amount="1d4x1000",
            .percent_chance=30,
            .is_per_individual=false
        },
        .platinum={
            .amount="1d6x100",
            .percent_chance=30,
            .is_per_individual=false
        },
        .gems={
            .amount="10d6",
            .percent_chance=55,
            .is_per_individual=false
        },
        .jewelry={
            .amount="5d6",
            .percent_chance=50,
            .is_per_individual=false
        },
        .maps_or_magic={
            .percent_chance=50,
            .types={
                {
                    .amount=3,
                    .variable_amount="",
                    .is_map_possible=false,
                    .possible_magic_items=ANY_MAGIC_ITEM
                }
            },
            .types_count=1
        }
    }
};


static char *
describe_coins_gems_or_jewelry(struct coins_gems_or_jewelry *coins_gems_or_jewelry);

static char *
describe_maps_or_magic(struct maps_or_magic *maps_or_magic);

static void
generate_treasure_coins(int *coins,
                        struct rnd *rnd,
                        struct coins_gems_or_jewelry *coins_type,
                        int individual_count);

static void
generate_treasure_gems(struct Treasure *treasure, struct rnd *rnd);

static void
generate_treasure_jewelry(struct Treasure *treasure, struct rnd *rnd);

static void
generate_treasure_maps_or_magic(struct Treasure *treasure, struct rnd *rnd);

static void
generate_treasure_maps_or_magic_type(struct maps_or_magic_type *type,
                                     struct Treasure *treasure,
                                     struct rnd *rnd);

static char const *
possible_maps_or_magic_name(bool is_map_possible,
                            PossibleMagicItems possible_magic_items);


char *
treasure_type_alloc_description(struct treasure_type *treasure_type,
                                bool include_header)
{
    /*          1000       1000       1000       1000       100                          */
    /* Type    copper     silver    electrum     gold     platinum     gems      jewelry */
    /*  A     1-6 :25%   1-6 :30%   1-6 :35%   1-10:40%   1-4 :25%   4-40:60%   3-30:50% */
    /*                                                                                   */
    /*  J    3-24 pieces                                                                 */
    /*      per individual                                                               */
    static char const *header =
    "          |  1,000's  |  1,000's  |  1,000's  |  1,000's  |   100's   |           |           |    Maps   \n"
    "Treasure  |    of     |    of     |    of     |    of     |    of     |           |           |     or    \n"
    "  Type    |  Copper   |  Silver   | Electrum  |   Gold    | Platinum  |   Gems    |  Jewelry  |   Magic   \n"
    "----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------\n";
    char *letter_format = "    %c    ";
    
    char *letter;
    char *copper = describe_coins_gems_or_jewelry(&treasure_type->copper);
    char *silver = describe_coins_gems_or_jewelry(&treasure_type->silver);
    char *electrum = describe_coins_gems_or_jewelry(&treasure_type->electrum);
    char *gold = describe_coins_gems_or_jewelry(&treasure_type->gold);
    char *platinum = describe_coins_gems_or_jewelry(&treasure_type->platinum);
    char *gems = describe_coins_gems_or_jewelry(&treasure_type->gems);
    char *jewelry = describe_coins_gems_or_jewelry(&treasure_type->jewelry);
    char *maps_or_magic = describe_maps_or_magic(&treasure_type->maps_or_magic);
    
    letter = str_alloc_formatted(letter_format, treasure_type->letter);
    
    char *format = "%s%s | %s | %s | %s | %s | %s | %s | %s | %s\n";
    char *description = str_alloc_formatted(format,
                                            (include_header ? header : ""),
                                            letter, copper, silver, electrum,
                                            gold, platinum, gems, jewelry,
                                            maps_or_magic);
    free_or_die(letter);
    free_or_die(copper);
    free_or_die(silver);
    free_or_die(electrum);
    free_or_die(gold);
    free_or_die(platinum);
    free_or_die(gems);
    free_or_die(jewelry);
    free_or_die(maps_or_magic);
    
    return description;
}


static char *
describe_coins_gems_or_jewelry(struct coins_gems_or_jewelry *coins_gems_or_jewelry)
{
    if (!coins_gems_or_jewelry->percent_chance) return strdup_or_die("   nil   ");

    if (coins_gems_or_jewelry->is_per_individual) {
        struct dice amount = dice_parse(coins_gems_or_jewelry->amount);
        char *range = dice_alloc_base_range_description(amount);
        char *description = str_alloc_centered_and_formatted(9, "%s per",
                                                             range);
        free_or_die(range);
        return description;
    } else {
        struct dice amount = dice_parse(coins_gems_or_jewelry->amount);
        char *range = dice_alloc_base_range_description(amount);
        char *description = str_alloc_centered_and_formatted(9, "%s:%2i%%",
                                                             range,
                                                             coins_gems_or_jewelry->percent_chance);
        free_or_die(range);
        return description;
    }
}


static char *
describe_maps_or_magic(struct maps_or_magic *maps_or_magic)
{
    if (!maps_or_magic->percent_chance) return strdup_or_die("   nil   ");
    
    char *type_descriptions[6] = {};
    for (int i = 0; i < maps_or_magic->types_count; ++i) {
        struct maps_or_magic_type *type = &maps_or_magic->types[i];
        char const *type_name = possible_maps_or_magic_name(type->is_map_possible,
                                                            type->possible_magic_items);
        if (str_not_empty(type->variable_amount)) {
            struct dice variable_amount = dice_parse(type->variable_amount);
            char *range = dice_alloc_range_description(variable_amount);
            type_descriptions[i] = str_alloc_formatted("%s %ss",
                                                       range, type_name);
            free_or_die(range);
        } else if (   type->is_map_possible
                   && type->possible_magic_items == ANY_MAGIC_ITEM)
        {
            type_descriptions[i] = str_alloc_formatted("any %i", type->amount);
        } else if (type->possible_magic_items == NON_WEAPON_MAGIC) {
            type_descriptions[i] = str_alloc_formatted("any %i except sword or misc weapon",
                                                       type->amount);
        } else if (type->possible_magic_items == ANY_MAGIC_ITEM) {
            type_descriptions[i] = str_alloc_formatted("any %i magic",
                                                       type->amount);
        } else {
            char const *plural = (type->amount == 1) ? "" : "s";
            type_descriptions[i] = str_alloc_formatted("%i %s%s",
                                                       type->amount,
                                                       type_name,
                                                       plural);
        }
    }

    char *description = NULL;
    for (int i = 0; i < maps_or_magic->types_count; ++i) {
        char const *separator = (i) ? ", " : "";
        str_realloc_append_formatted(&description, "%s%s",
                                     separator, type_descriptions[i]);
        free_or_die(type_descriptions[i]);
    }
    str_realloc_append_formatted(&description, ": %i%%",
                                 maps_or_magic->percent_chance);
    return description;
}


void
treasure_generate(struct treasure_type *treasure_type,
                  struct Treasure *treasure,
                  struct rnd *rnd,
                  int individual_count)
{
    treasure->type = treasure_type;
    
    generate_treasure_coins(&treasure->coins.cp, rnd,
                            &treasure_type->copper, individual_count);
    generate_treasure_coins(&treasure->coins.sp, rnd,
                            &treasure_type->silver, individual_count);
    generate_treasure_coins(&treasure->coins.ep, rnd,
                            &treasure_type->electrum, individual_count);
    generate_treasure_coins(&treasure->coins.gp, rnd,
                            &treasure_type->gold, individual_count);
    generate_treasure_coins(&treasure->coins.pp, rnd,
                            &treasure_type->platinum, individual_count);
    generate_treasure_gems(treasure, rnd);
    generate_treasure_jewelry(treasure, rnd);
    generate_treasure_maps_or_magic(treasure, rnd);
}


static void
generate_treasure_coins(int *coins,
                        struct rnd *rnd,
                        struct coins_gems_or_jewelry *coins_type,
                        int individual_count)
{
    *coins = 0;
    if (!coins_type->percent_chance) return;
    if (coins_type->is_per_individual) {
        assert(individual_count > 0);
        for (int i = 0; i < individual_count; ++i) {
            *coins += roll(coins_type->amount, rnd);
        }
    } else {
        int percent_score = roll("1d100", rnd);
        if (percent_score <= coins_type->percent_chance) {
            *coins = roll(coins_type->amount, rnd); // * coins_type->multiplier;
        }
    }
}


static void
generate_treasure_gems(struct Treasure *treasure, struct rnd *rnd)
{
    treasure->gems = NULL;
    treasure->gemsCount = 0;
    if (!treasure->type->gems.percent_chance) return;
    int percent_score = roll("1d100", rnd);
    if (percent_score <= treasure->type->gems.percent_chance) {
        treasure->gemsCount = roll(treasure->type->gems.amount, rnd);
        treasure->gems = calloc_or_die(treasure->gemsCount, sizeof(struct gem));
        for (int i = 0; i < treasure->gemsCount; ++i) {
            gem_initialize(&treasure->gems[i]);
            gem_generate(&treasure->gems[i], rnd);
        }
    }
}


static void
generate_treasure_jewelry(struct Treasure *treasure, struct rnd *rnd)
{
    treasure->jewelry = NULL;
    treasure->jewelryCount = 0;
    if (!treasure->type->jewelry.percent_chance) return;
    int percent_score = roll("1d100", rnd);
    if (percent_score <= treasure->type->jewelry.percent_chance) {
        treasure->jewelryCount = roll(treasure->type->jewelry.amount, rnd);
        treasure->jewelry = calloc_or_die(treasure->jewelryCount,
                                          sizeof(struct jewelry));
        for (int i = 0; i < treasure->jewelryCount; ++i) {
            jewelry_initialize(&treasure->jewelry[i]);
            jewelry_generate(&treasure->jewelry[i], rnd);
        }
    }
}


static void
generate_treasure_maps_or_magic(struct Treasure *treasure, struct rnd *rnd)
{
    if (!treasure->type->maps_or_magic.percent_chance) return;
    int percent_score = roll("1d100", rnd);
    if (percent_score <= treasure->type->maps_or_magic.percent_chance) {
        for (int i = 0; i < treasure->type->maps_or_magic.types_count; ++i) {
            struct maps_or_magic_type *type = &treasure->type->maps_or_magic.types[i];
            generate_treasure_maps_or_magic_type(type, treasure, rnd);
        }
    }
}


static void
generate_treasure_maps_or_magic_type(struct maps_or_magic_type *type,
                                     struct Treasure *treasure,
                                     struct rnd *rnd)
{
    int amount = type->amount;
    if (str_not_empty(type->variable_amount)) {
        amount = roll(type->variable_amount, rnd);
    }
    
    int magic_items_count = 0;
    int maps_count = 0;
    for (int i = 0; i < amount; ++i) {
        if (type->is_map_possible) {
            bool is_magic_item_possible = type->possible_magic_items & ANY_MAGIC_ITEM;
            if (is_magic_item_possible) {
                (roll("1d100", rnd) <= 10) ? ++maps_count : ++magic_items_count;
            } else {
                ++maps_count;
            }
        } else {
            ++magic_items_count;
        }
    }
    
    if (maps_count) {
        generateMapsForTreasure(treasure, rnd, maps_count);
    }
    if (magic_items_count) {
        generateMagicItemsForTreasure(treasure, rnd, magic_items_count,
                                      type->possible_magic_items);
    }
}


static char const *
possible_maps_or_magic_name(bool is_map_possible,
                            PossibleMagicItems possible_magic_items)
{
    if (is_map_possible) {
        switch (possible_magic_items) {
            case NO_MAGIC_ITEM: return "map";
            case ANY_MAGIC_ITEM: return "any";
            case NON_WEAPON_MAGIC: return "map or non weapon magic";
            default: 
                fail("possible_magic_items = %0x", possible_magic_items);
                return "(map or magic)";
        }
    } else {
        switch (possible_magic_items) {
            case POTION: return "potion";
            case SCROLL: return "scroll";
            case RING: return "ring";
            case ROD_STAFF_WAND: return "rod/staff/wand";
            case MISC_MAGIC: return "misc magic";
            case ARMOR_SHIELD: return "armor/shield";
            case SWORD: return "sword";
            case MISC_WEAPON: return "misc weapon";
                
            case ANY_MAGIC_ITEM: return "any magic";
            case MAGIC_WEAPON_OR_ARMOR: return "sword, armor or misc weapon";
            default: 
                fail("possible_magic_items = %0x", possible_magic_items);
                return "(magic)";
        }
    }
}


struct treasure_type *
treasure_type_by_letter(char letter)
{
    assert(letter >= 'A' && letter <= 'Z');
    return &treasure_types[letter - 'A'];
}
