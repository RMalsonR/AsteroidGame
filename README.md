# AsteroidGame

Asteroid Game Project  
From example is [here](https://www.youtube.com/watch?v=_TKiRvGfw3Q)

___

## Requirements  
For linux / macos sytems.
* g++ 
* cmake 
* libx11-dev 
* (for macos) see also installing /opt/X11/

___

## TODO List

* Add sounds
* UI for change complexity, play, sound and another
* Use database in docker-compose to store all game data
* Refactoring too complexity parts
___

## Logic explanation


#### Control:
* UP key - ship will move forward
* LEFT, RIGHT keys - ship will rotate
* SPACE - shooting
* ESCAPE - exit game
  
#### Points:

* Largest asteroid give 100 points. Middle 200 and smallest 400.
* You will get extra points after completed level (this value increases by 1.5 after each level)
* The faster you complete the level, the more points you get.

#### Win condition:

* For win game you need to complete all **5 levels**.
* You will lose score that you collect at failed level.
* After screen (**YOU WIN** or **GAME OVER**) press **SPACE** to reset game