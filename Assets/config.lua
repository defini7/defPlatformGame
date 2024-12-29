local tile_size = { 16, 16 }

local function CharToTileType(char)
    if char == '.' then return TileType.Empty end
    if char == '#' then return TileType.Dirt end
    if char == 'x' then return TileType.Grass end
    if char == 'o' then return TileType.Coin end
end

local function MapStringToTiletypes(map, width, height)
    local output = {}

    for y=0, height do
        for x=1, width do
            local i = y * width + x
            output[i] = CharToTileType(map:sub(i, i))
        end
    end

    return output
end

-- Won't look good if you change it, will be fixed
Window = { ScreenWidth=512, ScreenHeight=240,
			PixelWidth=2, PixelHeight=2 }

local map_home = "...........o...................."..
                 "................................"..
                 "..........o.o..................."..
                 ".....o...xxxxx.................."..
                 "...................x......x....."..
                 "...............x...#......#....."..
                 "xxxxxxxxxxxxxxx#xxx#xxxxxx#xxxxx"..
                 "################################"..
                 "################################"..
                 "################################"

local map_next = "................................"..
                 "................................"..
                 "................................"..
                 "................................"..
                 "................................"..
                 "................................"..
                 "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"..
                 "################################"..
                 "################################"..
                 "################################"


Assets = {

    Tiles = {

        Size = tile_size,

        FileOffsets = {
            [TileType.Coin ] = { 2 * tile_size[1], 0 },
            [TileType.Grass] = { 3 * tile_size[1], 0 },
            [TileType.Dirt ] = { 4 * tile_size[1], 0 }
        }
    },

    Sprites = {
        Tiles = "Assets/tiles.png",
        Background = "Assets/sky.png"
    },

    Levels = {

        [1] = {
            Data = MapStringToTiletypes(map_home, 32, 10),
            Size = { 32, 10 }
        },

        [2] = {
            Data = MapStringToTiletypes(map_next, 32, 10),
            Size = { 32, 10 }
        }
    }
}

Game = {

    Velocity = {
        Epsilon = 2,
        Friction = 3,
        FallSpeed = 20,
        Min = { -10, -100 },
        Max = { 10, 100 },

        Player = {
            GroundSpeed = 12,
            AirSpeed = 10,
            JumpSpeed = 11
        },

        Enemy = {
            GroundSpeed = 8,
            AirSpeed = 6,
            JumpSpeed = 5
        }
    }
}

function Initialise()
    SpawnEnemy(1, 12, 4)
    SpawnEnemy(1, 14, 4)
    SpawnEnemy(1, 17, 4)
    SpawnEnemy(1, 20, 4)
    SpawnEnemy(1, 23, 4)
    SpawnEnemy(2, 20, 4)

    -- we spawn player after spawning enemies,
    -- because otherwise it will invalidate player's
    -- iterator in vecDynamics vector
    SpawnPlayer(1, 1, 4)
end
