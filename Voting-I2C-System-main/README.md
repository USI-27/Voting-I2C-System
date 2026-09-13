# 8051 Electronic Voting Machine

An Electronic Voting Machine (EVM) implemented using the **8051 microcontroller (AT89C51)**.  
The system records votes using **EEPROM via I2C communication** and displays results on a **16x2 LCD**.

---

## Features

- Voting for **4 candidates**
- **Duplicate vote prevention**
- **EEPROM vote storage** (data retained after power loss)
- **LCD interface** for instructions and results
- **Winner detection and tie handling**
- **Reset functionality**

---

## Tools Used

- Keil µVision
- Proteus 8 Professional

---

## Hardware Components

- AT89C51 (8051 microcontroller)
- AT24C512 EEPROM
- 16x2 LCD
- Push buttons
- Crystal oscillator
- Resistors and capacitors

---

## Running the Simulation

1. Open the Keil project file:

   VotingI2CAdvanced.uvproj

2. Build the project in Keil µVision to generate the firmware file:

   VotingI2CAdvanced.hex

3. Open the Proteus simulation project:

   Voting3.pdsprj

4. In Proteus:
   - Double-click the **AT89C51 microcontroller**
   - In the **Program File** field, browse and select:
     
     VotingI2CAdvanced.hex

5. Start the simulation to run the Electronic Voting Machine.

---

## Author

Mohd. Jawwad Rizvi  
B.Tech – Electronics and Communication Engineering  
LNMIIT Jaipur


