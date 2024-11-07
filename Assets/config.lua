local tile_size = { 32, 32 }

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
Window = { ScreenWidth=256, ScreenHeight=192,
			PixelWidth=4, PixelHeight=4 }

local map_home = "........xxxxxxxx"..
                 ".....ooo........"..
                 "................"..
                 "..xxxxxxxx...xxx"..
                 "..########...###"..
                 "..########..x###"..
                 "................"..
                 "xxxxxxxxxxxxxxxx"

local map_next = ".xx.....x......."..
                 ".....ooo........"..
                 "...o..........o."..
                 ".....xxxxx...xxx"..
                 "......###....###"..
                 "..xxx..........."..
                 "................"..
                 "xxxxxxxxxxxxxxxx"

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
        Tiles = "../../../../../Assets/tiles.png",
        Background = "../../../../../Assets/sky.png"
    },
    Levels = {
        Home = {
            Data = MapStringToTiletypes(map_home, 16, 8),
            Size = { 16, 8 }
        },
        Next = {
            Data = MapStringToTiletypes(map_next, 16, 8),
            Size = { 16, 8 }
        }
    }
}

Game = {
    Velocity = {
        Epsilon = 1,
        Friction = 4,
        GroundSpeed = 15,
        AirSpeed = 15.0,
        JumpSpeed = 12,
        FallSpeed = 20,
        Min = { -10, -100 },
        Max = { 10, 100 }
    }
}
