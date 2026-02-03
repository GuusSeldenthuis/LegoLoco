# openLegoLoco - Development Roadmap

Recreating LEGO Loco (1998) in C++ with raylib.

## Milestone 1: Core Engine Foundation

### 1.1 Isometric Grid System
- [ ] Implement isometric coordinate conversion (screen <-> world)
- [ ] Render isometric grid with proper depth sorting
- [ ] Camera panning with mouse drag
- [ ] Camera zoom in/out

### 1.2 Tile System
- [ ] Define tile data structure (type, orientation, connections)
- [ ] Load and render placeholder tile sprites
- [ ] Implement tile placement on grid
- [ ] Tile removal/replacement

### 1.3 World Management
- [ ] World data structure (2D array of tiles)
- [ ] Save world to file
- [ ] Load world from file

---

## Milestone 2: Buildings & Objects

### 2.1 Building Placement
- [ ] Building data structure (size, type, properties)
- [ ] Multi-tile building footprints
- [ ] Placement validation (no overlap)
- [ ] Building removal

### 2.2 Core Buildings
- [ ] Houses (minifigure spawn points)
- [ ] Train station (passenger boarding)
- [ ] Train depot (train spawn/storage)
- [ ] Decorative buildings

### 2.3 Scenery
- [ ] Trees and vegetation
- [ ] Roads (visual only for v1)
- [ ] Fences and props

---

## Milestone 3: Track System

### 3.1 Track Tiles
- [ ] Straight track (horizontal/vertical in isometric)
- [ ] Curved track (4 orientations)
- [ ] Track validation (connections must align)
- [ ] Visual feedback for invalid placement

### 3.2 Track Pathfinding
- [ ] Build track graph from placed tiles
- [ ] Calculate path along connected tracks
- [ ] Handle junctions and switches
- [ ] Detect loops and dead ends

---

## Milestone 4: Minifigures

### 4.1 Basic Minifigure
- [ ] Minifigure sprite with animations (idle, walk)
- [ ] Spawn from houses
- [ ] Random wandering AI
- [ ] Pathfinding on walkable areas

### 4.2 Minifigure Interaction
- [ ] Pick up minifigure with mouse
- [ ] Drop minifigure at new location
- [ ] Minifigure walks to nearest station
- [ ] Board train at station

### 4.3 Passenger System
- [ ] Minifigures wait at stations
- [ ] Board train when it stops
- [ ] Exit at destination station
- [ ] Track passenger count per train

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
- [ ] Carriage connection physics (follow with delay)

### 5.3 Train Control
- [ ] Select train by clicking
- [ ] Control panel UI for selected train
- [ ] Reverse train direction
- [ ] Multiple trains on separate track sections

---

## Milestone 6: User Interface

### 6.1 Toy Box (Build Menu)
- [ ] Toggle build mode on/off
- [ ] Category tabs (tracks, buildings, scenery)
- [ ] Item selection with preview
- [ ] Rotation control for selected item

### 6.2 Game HUD
- [ ] Current tool/selection indicator
- [ ] Mini-map (optional for v1)
- [ ] Train list panel
- [ ] Pause/play simulation toggle

### 6.3 Interaction Feedback
- [ ] Hover highlights on tiles
- [ ] Selection indicators
- [ ] Placement ghost preview
- [ ] Error messages for invalid actions

---

## Milestone 7: Simulation Loop

### 7.1 Game States
- [ ] Build mode (simulation paused, can edit)
- [ ] Play mode (simulation running)
- [ ] Seamless switching between modes

### 7.2 Time System
- [ ] Day/night cycle (visual only)
- [ ] Simulation tick rate
- [ ] Pause/speed controls

---

## Milestone 8: Polish & First Playable

### 8.1 Audio
- [ ] Background music
- [ ] Train sounds (moving, whistle, stopping)
- [ ] UI click sounds
- [ ] Ambient sounds

### 8.2 Visual Polish
- [ ] Replace placeholder sprites with LEGO-style art
- [ ] Particle effects (steam, smoke)
- [ ] Smooth camera transitions
- [ ] Animation polish

### 8.3 Quality of Life
- [ ] Undo/redo for building
- [ ] Bulldozer tool (mass removal)
- [ ] Keyboard shortcuts
- [ ] Settings menu (volume, resolution)

---

## Future Milestones (Post-v1)

- Multiplayer (LAN)
- Postcard system
- More building types
- Train customization
- Minifigure moods and needs
- Seasons and weather
- Easter eggs from original game
