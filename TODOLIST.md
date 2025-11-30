# TODO LIST

## High Priority

- [x] [Thaís] Player movement
    - [x] keyboard WASD
    - [x] gamepad 
- [ ] [Juan] Create enemy
    - [x] Basic static
    - [x] Patrol mode
    - [x] Detects player
        - [x] Dead zone (cone-like detection, ignores player if they are behind them)
    - [x] Chases player
        - [ ] Test obstacle handling
    - [x] Attacks player
        - [ ] Test attack animation
        - [ ] Test cooldown
        - [ ] Player death
    - [ ] Flexiblility for bosses and spawning animation (skills vs state machine)(complex)
    - [x] (maybe) Detection radius smaller than give-up-from-persuit radius
    - [x] (maybe) Waits for a while before turning on patrol
    - [x] (maybe) Memory of last seen player position
    - [x] (maybe) Return to original patrol area
- [ ] [Juan] Player basic combat 
    - [ ] basic
    - [ ] charged
    - [ ] special
    - [ ] dodge and invulnerability frame
- [ ] [Juan] Implement camera follow and room bounds
- [ ] [_____] Load levels from CSV/TMX
    - [ ] Forest
    - [ ] Surroundings
    - [ ] City
    - [ ] Castle
- [ ] [Filipe] Implement collision and physics system
- [ ] [Filipe] Implement player abilities and upgrades
- [ ] [_____] Implement boss
- [ ] [_____] Implement spawn system and wave balancing
- [ ] [Filipe] Implement health and roguelike run system
- [x] [Thaís] Player basic sprit and animations

## Mid Priority

- [ ] [Pains] Implement HUD
    - [ ] Lives / HP display (hearts or bar)
    - [ ] Item slots (icons + count)
    - [ ] Active item indicator + usage key
    - [ ] Cooldown timers (visual overlays)
    - [ ] Run timer / Score display
    - [ ] Boss HP bar (show during boss fight)
    - [ ] Status effect icons (poison, slow, buffs)
    - [ ] Responsive layout (different resolutions)
- [ ] [Filipe] Implement item pickups
    - [ ] Active
    - [ ] Passive
- [ ] [Filipe] Implement item selection UI / loot choices
- [ ] [Pains] Implement menus
- [ ] [Pains] Integrate audio
- [ ] [_____] Implement particle systems and visual feedback
- [ ] [_____] Integrate shaders and lighting effects
- [ ] [Thais] Finalize sprites, tilesets and animations
- [ ] [Pains] Main Menu (detailed)
    - [ ] Start Run
    - [ ] Tutorial (first-run trigger)
    - [ ] Options (Audio, FullScreen, Controls?, Accessibility?)
    - [ ] Credits
    - [ ] Exit
- [ ] [Pains] In-Game Pause Menu (detailed)
    - [ ] Resume
    - [ ] Options (Audio, Controls, Accessibility)
    - [ ] Keybind remap
    - [ ] Restart Run (confirm)
    - [ ] Exit to Main Menu (confirm)
    - [ ] View Run Stats (time, enemies defeated, items picked)
- [ ] [Filipe] Resize the game
    - [ ] Increase all
    - [x] Implement full screen mode

## Low Priority

- [ ] [_____] Save settings and persistent progression
- [ ] [_____] Performance profiling and memory fixes
- [ ] [_____] Write documentation and README
- [ ] [_____] Build scripts, CI and packaging
- [ ] [_____] Procedural enemy and item placement

## Backlog (if we have time)

- [ ] [_____] Procedural generation of maps
- [ ] [_____] Point of conflict where we choose to be good or evil
- [ ] [_____] Rainbow cat
