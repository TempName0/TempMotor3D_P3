﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Path
{
    public enum PUZZLE_DIRECTION
    {
        DIR_NORTH = 0,
        DIR_EAST = 1,
        DIR_SOUTH = 2,
        DIR_WEST = 3
    };

    public int[,] walkability;
    // 0: Walkable
    // 1: No walkable
    public int width = 0;
    public int height = 0;

    public PUZZLE_DIRECTION puzzle_direction = PUZZLE_DIRECTION.DIR_NORTH;

    public Path(int _width, int _height, int puzzle_orientation)
    {
        width = _width;
        height = _height;
        puzzle_direction = (PUZZLE_DIRECTION) puzzle_orientation;

        walkability = new int[width, height];
    }

    // ---------------------------------

    public void CreateWalkability(int path_index)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                // Set all tiles to no walkable at first.
                walkability[x, y] = 1;
            }
        }

         switch (path_index)
        {
            case 0:
                walkability[0, 0] = 0;
                walkability[1, 0] = 0;
                walkability[1, 1] = 0;
                walkability[2, 1] = 0;
                walkability[2, 2] = 0;
                walkability[2, 3] = 0;
                walkability[3, 3] = 0;
                walkability[3, 4] = 0;
                walkability[3, 5] = 0;
                /*SetWalkability(0, 0, 0);
                SetWalkability(1, 0, 0);
                SetWalkability(1, 1, 0);
                SetWalkability(2, 1, 0);
                SetWalkability(2, 2, 0);
                SetWalkability(2, 3, 0);
                SetWalkability(3, 3, 0);
                SetWalkability(3, 4, 0);
                SetWalkability(3, 5, 0);*/
                break;
            case 1:
                walkability[4, 0] = 0;
                walkability[4, 1] = 0;
                walkability[4, 2] = 0;
                walkability[3, 2] = 0;
                walkability[3, 3] = 0;
                walkability[2, 3] = 0;
                walkability[2, 4] = 0;
                walkability[2, 5] = 0;
                /*SetWalkability(4, 0, 0);
                SetWalkability(4, 1, 0);
                SetWalkability(4, 2, 0);
                SetWalkability(3, 2, 0);
                SetWalkability(3, 3, 0);
                SetWalkability(2, 3, 0);
                SetWalkability(2, 4, 0);
                SetWalkability(2, 5, 0);*/
                break;
            case 2:
                walkability[2, 0] = 0;
                walkability[2, 1] = 0;
                walkability[2, 2] = 0;
                walkability[2, 3] = 0;
                walkability[3, 3] = 0;
                walkability[4, 3] = 0;
                walkability[5, 3] = 0;
                walkability[5, 4] = 0;
                walkability[5, 5] = 0;
                /*SetWalkability(2, 0, 0);
                SetWalkability(2, 1, 0);
                SetWalkability(2, 2, 0);
                SetWalkability(2, 3, 0);
                SetWalkability(3, 3, 0);
                SetWalkability(4, 3, 0);
                SetWalkability(5, 3, 0);
                SetWalkability(5, 4, 0);
                SetWalkability(5, 5, 0);*/
                break;
            case 3:
                walkability[5, 0] = 0;
                walkability[5, 1] = 0;
                walkability[4, 1] = 0;
                walkability[3, 1] = 0;
                walkability[2, 1] = 0;
                walkability[1, 1] = 0;
                walkability[1, 2] = 0;
                walkability[1, 3] = 0;
                walkability[2, 3] = 0;
                walkability[3, 3] = 0;
                walkability[4, 3] = 0;
                walkability[4, 4] = 0;
                walkability[5, 4] = 0;
                walkability[5, 5] = 0;
                /*SetWalkability(5, 0, 0);
                SetWalkability(5, 1, 0);
                SetWalkability(4, 1, 0);
                SetWalkability(3, 1, 0);
                SetWalkability(2, 1, 0);
                SetWalkability(1, 1, 0);
                SetWalkability(1, 2, 0);
                SetWalkability(1, 3, 0);
                SetWalkability(2, 3, 0);
                SetWalkability(3, 3, 0);
                SetWalkability(4, 3, 0);
                SetWalkability(4, 4, 0);
                SetWalkability(5, 4, 0);
                SetWalkability(5, 5, 0);*/
                break;
        }

        /**
        0 _____________________width
        |
        |
        |
        |
        |
        |
        |
        |
        height
         */
    }

    public void SetWalkability(int x, int y, int value)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            switch (puzzle_direction)
            {
                case PUZZLE_DIRECTION.DIR_NORTH:
                    SetNorthWalkability(x, y, value);
                    break;
                case PUZZLE_DIRECTION.DIR_EAST:
                    SetEastWalkability(x, y, value);
                    break;
                case PUZZLE_DIRECTION.DIR_SOUTH:
                    SetSouthWalkability(x, y, value);
                    break;
                case PUZZLE_DIRECTION.DIR_WEST:
                    SetWestWalkability(x, y, value);
                    break;
            }
        }
    }

    private void SetNorthWalkability(int x, int y, int value)
    {
        walkability[x, y] = value;
    }

    private void SetEastWalkability(int x, int y, int value)
    {
        int x1 = (width - 1) - y;
        int y1 = x;
        walkability[x1, y1] = value;
    }

    private void SetSouthWalkability(int x, int y, int value)
    {
        int x1 = (width - 1) - x;
        int y1 = (height - 1) - y;
        walkability[x1, y1] = value;
    }

    private void SetWestWalkability(int x, int y, int value)
    {
        int x1 = y;
        int y1 = (height - 1) - x;
        walkability[x1, y1] = value;
    }
}
