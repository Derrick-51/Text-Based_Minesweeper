# Text-Based_Minesweeper
This is a simple minesweeper clone that adds powers to the game.

## Input
Input consists of a string of characters that give commands.<br>
Letters are not case sensitive, but must adhere to the proper format.<br>

Ex. E 4C<br>
This will expose the zone at row 4, column C (shown below).<br><br>

              A   B   C   D   E   F   G   H 
            --------------------------------
       
        1|   [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
        2|   [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
        3|   [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
        4|   [ ] [ ] [X] [ ] [ ] [ ] [ ] [ ]
        5|   [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
        6|   [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
        7|   [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
        8|   [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
 
 
 ## Powers
 **There are four randomly selected powers:**<br>
 1. Horizontal Blast (Blasts 5 zones in a horizontal line)<br>
 2. Vertical Blast (Blasts 5 zones in vertical line)<br>
 3. Radial Blast (Blasts selected zone and all adjacent zones)<br>
 4. Jamboree (Shuffles mine positions and randomly redistributes exposed zones as blasts and marks targeted mines)<br><br>

**Zone Blast**<br>
 When a zone is blasted, it is exposed and its mine is removed.<br>
 Every blast has a chance to scorch the corresponding zone, leading to a loss of a mine count.<br>
 Powers have no limit on uses, because it's fun...<br>
