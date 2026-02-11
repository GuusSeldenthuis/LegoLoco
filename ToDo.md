# openLegoLoco - Development Roadmap

Recreating LEGO Loco (1998) in C++ with raylib.

## Milestone 1: Core Engine Foundation

### 1.1 2D Grid System
- [x] Implement coordinate conversion (screen <-> world)
- [x] Render grid with proper depth sorting
- [x] Camera panning with mouse drag
- [x] Camera zoom in/out

### 1.2 Tile System
- [x] Define tile data structure (type, connections)
- [x] Add tile orientation and connection metadata
- [x] Load and render placeholder tile sprites
- [x] Implement tile placement on grid
- [x] Tile removal/replacement
- [x] Road tile auto-connection logic
- [x] Train track manual placement with rotation

### 1.3 World Management
- [x] World data structure (2D array of tiles)
- [x] Save world to file
- [x] Load world from file

---

## Milestone 2: User Interface

### 2.1 Game States
- [ ] Main menu screen (New Game, Load Game, Quit)
- [ ] Transition from main menu to gameplay
- [ ] In-game state management (building vs playing)

### 2.2 Toy Box (Build Menu) - 3x5 button grid + close bar
The Toy Box is a floating panel with a 3-wide x 5-high button grid:

**Row 1 - Placement**
- [ ] Tiles button (opens sub-menu: tracks, road pieces)
- [ ] Buildings button (opens sub-menu: houses, stations, shops)
- [ ] Nature button (opens sub-menu: trees, vegetation, props)

**Row 2 - Removal**
- [ ] Eraser (remove single object on click)
- [ ] Bomb (destroy all - clear entire world)
- [ ] Undo (revert last action)

**Row 3 - File**
- [ ] Load/Save button (save and load world files)
- [ ] Background selector (change world background)
- [ ] Bulldozer mode toggle (auto-remove when placing on top)

**Row 4 - Settings**
- [ ] Windowed/Fullscreen toggle
- [ ] Help screen
- [ ] Audio on/off toggle

**Row 5 - Close**
- [ ] Close Toy Box button (single wide button spanning full width)

### 2.3 Interaction Feedback
- [x] Hover highlights on tiles
- [x] Placement ghost preview
- [ ] Selection indicators
- [ ] Error messages for invalid actions

---

## Milestone 3: Buildings & Objects

### 3.1 Building Placement
- [x] Building data structure (size, type, properties)
- [x] Multi-tile building footprints
- [x] Placement validation (no overlap)
- [x] Building removal

### 3.2 Core Buildings
- [ ] Houses (minifigure spawn points)
- [ ] Train station (passenger boarding)
- [ ] Train depot (train spawn/storage)
- [ ] Decorative buildings

### 3.3 Scenery
- [ ] Trees and vegetation
- [ ] Fences and props

---

## Milestone 4: Track System

### 4.1 Track Tiles
- [x] Straight track with manual rotation
- [x] Curved track (3x3) with manual rotation
- [ ] Track switches/junctions
- [ ] Track validation (connections must align)

### 4.2 Track Pathfinding
- [x] PathGraph for path tiles
- [ ] Extend PathGraph to track tiles
- [ ] Calculate path along connected tracks
- [ ] Handle junctions and switches
- [ ] Detect loops and dead ends

---

## Milestone 5: Trains

### 5.1 Basic Train Movement
- [ ] Train entity with position on track
- [ ] Smooth movement along track path
- [ ] Train speed control (accelerate/brake)
- [ ] Train stops at end of track

### 5.2 Train Composition
- [ ] Locomotive sprite
- [ ] Cargo/passenger carriage sprites
- [ ] Multiple carriages following locomotive

### 5.3 Train Control
- [ ] Select train by clicking
- [ ] Control panel UI for selected train
- [ ] Reverse train direction
- [ ] Multiple trains on separate track sections

---

## Milestone 6: Minifigures

### 6.1 Basic Minifigure
- [ ] Minifigure sprite with animations (idle, walk)
- [ ] Spawn from houses
- [ ] Random wandering AI
- [ ] Pathfinding on walkable areas

### 6.2 Minifigure Interaction
- [ ] Pick up minifigure with mouse
- [ ] Drop minifigure at new location
- [ ] Minifigure walks to nearest station
- [ ] Board train at station

### 6.3 Passenger System
- [ ] Minifigures wait at stations
- [ ] Board train when it stops
- [ ] Exit at destination station
- [ ] Track passenger count per train

---

## Milestone 7: Simulation Loop

### 7.1 Time System
- [ ] Simulation tick rate
- [ ] Pause/speed controls

---

## Milestone 8: Polish & First Playable

### 8.1 Audio
- [ ] Background music?
- [ ] Train sounds (moving, whistle)
- [ ] UI click sounds
- [ ] Ambient sounds

### 8.2 Visual Polish
- [ ] Particle effects (loc-steam, destruction animation)

### 8.3 Quality of Life
- [ ] Undo/redo for building
- [ ] Keyboard shortcuts?
- [ ] Settings menu (volume, resolution)
- [ ] That weird zoom-glass function

---

## Future Milestones (Post-v1)

- Multiplayer (LAN)
- Postcard system
- More building types
- Train customization
- Minifigure moods and needs
- Seasons and weather
- Easter eggs from original game
