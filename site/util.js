const ROMS = [
    "15PUZZLE",
    "BCTEST",
    "BLINKY",
    "BLITZ",
    "BRIX",
    "CONNECT4",
    "GUESS",
    "HIDDEN",
    "IBM",
    "INVADERS",
    "KALEID",
    "MAZE",
    "MERLIN",
    "MISSILE",
    "PONG",
    "PONG2",
    "PUZZLE",
    "SYZYGY",
    "TANK",
    "TETRIS",
    "TICTAC",
    "UFO",
    "VBRIX",
    "VERS",
    "WIPEOFF"
];

const loadRom = () => {
    const romSelector = document.getElementById("rom-selector");
    const romName = romSelector.options[romSelector.selectedIndex].value;
    Module.ccall('load_rom', null, ['string'], [`roms/${romName}.ch8`])
};

const listRom = () => {
    const romSelector = document.getElementById('rom-selector');
    romSelector.innerHTML = "";
    ROMS.forEach((romName) => {
        const option = document.createElement('option');
        option.value = romName;
        option.innerHTML = romName;
        romSelector.appendChild(option);
    });
};
