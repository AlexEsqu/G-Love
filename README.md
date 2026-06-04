https://app.notion.com/p/G-Love-Module-Electronique-06-26-42-Chips-371a25cc104180e3b6d8d438b70a2ca8


# Presentation

[G-Love] est un projet de conception d’un système de vêtement électronique (wearable) porté sur les bras., intégrant des bandes LEDSs RGB réactives à l’environnement et aux mouvements du porteur. (acceleration, flexion, pression)

Le système se présente sous forme de gant relié à une manchette d’avant-bras parcourus de capteurs et de LEDs. Un capteur sur le poignet permet de naviguer entre les modes.

---

# Table des matieres

---

# 1.Manuel d’utilisation

## Interactions

- Sur flexion / pression d’un doigt : animation localisée sur le segment correspondant
- Mouvement brusque du bras : traînée lumineuse, onde ?
- Orientation du bras : dégradé de couleur selon l’axe
- Bonus DMX : LEDs pilotees par reception DMX / Gestion d’une scene en transmission DMX

## Parametrage

- Pression bracelet : changement de mode pour passer d’un preset a l’autre
- Switch : pour demarrer avec ou sans feature LEDs
- Calibrage : via proximite aimant ou combinaison mouvement ?
- Bonus Touchscreen : Changer de mode / couleur

## Demarrage

Un utilisateur devra passer par une étape de calibration pour sa première utilisation du gant (longueur du bras et des doigts, amplitude de mouvement, calibration des senseurs…).
Mais les utilisations suivantes ne devraient pas nécessiter de vraie calibration, placer une nouvelle position d’origine au plus.

## Résumé

Les senseurs du gant devraient permettre de mesurer en temps réel la position de la main dans l’espace, la flexion des doigts, et les contacts avec le bout des doigts.

Les differents capteurs utilisés simultanément devraient permettre a l’utilisateur de déclencher plusieurs effets lumineux sur le display led, avec des combinaisons additives d’effets (mouvement + position des doigts X != mouvement + position Y != mouvement seul)

Ces mesures peuvent aussi être transmises en temps réel à une autre machine (à définir) pour lier ces mesures d’inputs à d’autres effets.

# 2.Features

---

## Mandatory

- 5 Ruban LED adressable sur les doigts, réagissant individuellement
- Interaction sur flexion d’un doigt (capteur flex a chaque doigt)
- Interaction sur pression du bout d’un doigt (capteur pression au bout de chaque doigt)
- Interaction sur :
    - mouvement de la main
    - l’orientation de la main
- Transmission coordonnée dans l’espace en temps réel par UART (précision dans la limite du matériel accessible avec le budget : plusieurs IMU, ou IMU plus précis, balise et triangulation…)
- Surface avant-bras LED adressable 16x16 / 16x64 (dans limite budget)
- Interaction sur l’avant bras (capteur pression)
- Envoi des donnees en USB via bridge USB to UART
- Système de reset des positions/mouvements (simple bouton ou combinaison d’input). Peut être nécessaire suivant le fonctionnement du capteur et la longueur d’utilisation.
- Système de calibration capable définir nouvelle origine (point 0,0,0) , sans reset tout le système. (si la calibration a été faite, un reset complet de la machine peut etre rattrappe en 2secondes si l’utilisateur garde une position de repos)

    Définir amplitude max du bras (bras tendu, leve, a plat sur les cuisses)

- Gestion embarquée de la chaleur des LEDs par capteur température et optimisation software ; utiliser des techno type textile conducteur, ??? pour permettre PLUS DE LED

## Bonus

#### Communication

- Interaction entre 2 gants dans le meme espace via coordonnees transmises en USB

#### TouchScreen

- Bonus: Touchscreen ou autre système d’input pour changer de mode/paramètres.

    Peut etre utile pour changer plus facilement les options pendant l‘utilisation de l’objet. Et pour visualiser les paramètres actuels sans trop perturber le display principal de l’objet.


#### DMX

- Bonus: Transmission DMX via driver RS-485 (MAX485 + XLR DMX)

    +Note: émission de format DMX probablement ingérable, réception semble plus réaliste.


#### FingerTip

- Bonus: Module Plug and Play HEARTBEAT capteur de pouls
- Bonus: Module Plug and play PISTODOIGT en infrarouge pour jouer au laser tag avec un doigt canon et le flex d’un autre doigt pour la gâchette  🔫
- Super Bonus : Module Plug and Play CURSOR permettant de dessiner en 3D ; Captation suffisamment précise pour dessiner un 42 lisible dans l’espace d’un 1m3

---

# 3. Schema fonctionnel

!Schema Fonctionnel.png

TO DO:
- formes logique
- légende
- ajouter ref compo


---

# 4. Matériel et protocoles

- **Microcontrôleurs relies UART** :
    - ATmega328P
    - ATmega32U4 (1 controleur principal pour les gouverner tous)
- **Bandes LEDs RGB** : WS2812B ou APA102 le long des doigts et panneau LEDs sur l’avant-bras
- **Capteurs** :

    **ADC** protocole **: Flex sensors** pour la flexion (duh), **FSR** (force-sensing resistor) pour la pression des doigts via multiplexeur CD4067

    **SPI / I2c** protocole **: IMU** (inertial measurement unit) pour accéléromètre + gyroscope,

    **Proximité** : Capteur Hall digital A3144

- **Support** : PCB Flex et PCB rigides avec segments flexibles entre chaque
- **Microcontrôleurs relies en UART** :
    - **ATmega328P (**main**)**
    - **ATmega2561** (avant-bras)
- **Capteurs** :

    **ADC** via multiplexer CD4067 **:**

    - **Flex sensors** pour la flexion des doigts
    - **FSR** (force-sensing resistor) pour la pression des doigts

    **I2C : IMU** (inertial measurement unit) pour accéléromètre + gyroscope

    **Digital I/O** :

    - **Capteur** Hall digital A3144
    - **bandes LEDs RGB** APA102 le long des doigts et panneau LEDs sur l’avant-bras
- **HeatSensor :** Utilisation de Thermistance NTC (negative temperature coefficient = la résistance diminue lorsque la température augmente), placé là où il y a le plus de variation de chaleur possible (rubans LED) avec de la colle thermique pour mieux capté la chaleur et recouvert d’un ruban Kapton pour isolé de l’air ambiant

- **Bonus SPI/I2C** : touchscreen, **GC9A01** est la puce qui contrôle l'affichage de l'écran et communique en SPI et **CST816D** est la puce qui contrôle la détection du tactile, communique en I2C
    - En sortie (Affichage - GC9A01) :
        - Afficher le mode d'interaction actuel (ex: "Mode DMX", "Mode Laser Tag")
        - Afficher des retours visuels lors de l'étape de calibration
    - En entrée (Saisie tactile - CST816D) :
        - Changement de mode: changement de mode pour différent interaction prédefini
        - Parametrage de couleurs: afficher une roue chromatique sur l'écran, permettant à l'utilisateur de sélectionner directement au doigt la couleur de base de ses bandes LED.
        - Bouton de secours / de reset: permet de déclencher manuellement un recalibration ou un “reset” en appuyant sur un bouton
- **Bonus UART** : USB
- **Bonus UART** : DMX
- **Support** : PCB Flex et PCB rigides avec segments flexibles entre chaque

## SPI (**Serial Peripheral Interface**)

SPI est un protocole ultra-rapide avec 4 fils : MOSI, MISO, SCK, CS. Fonctionnant sur une architecture de type master/slave, il permet des transferts de données bidirectionnels simultanenent  sans ralentir le microcontrôleur principal.

La synchronisation des échanges est assuré par un signal d’horloge généré exclusivement par le microcontroleur configuré en maitre

Le bus SPI repose sur l’utilisation de quatre lignes de signaux:

- SCK(Serial Clock) : Signal d’horloge generé par le maitre, il cadence le rythme de transmission des bits de données
- MOSI(Master Out Slave In) : Ligne de données unidirectionelle transportant les informations du maitre vers le slave
- MISO(Master In Slave Out) : ligne de données unidirectionelle transportant les informations du slave vers le maitre
- CS / SS (Chip Select / Slave Select) : Ligne d’activation dédié a chaque composant esclave, ce signal est actif a l’etat bas (0V), tant que le signal d’un esclave est haut, celui ci ignore le bus SPI et libere la ligne MISO
- Composant : Ruban LED APA102 (avant-bras).
    - **Le Maître :** L'ATmega328P est le maître du bus SPI
    - **Esclave :** Bandes LEDs APA102
    - **L'Usage :** Mise à jour à très haute fréquence de la matrice visuelle de l'avant-bras
    - **Câblage** : Utilise le D11(MOSI - Données) et D13 (SCK - Horloge)
- Composant : Ecran GC9A01
    - **Le Maitre** : L’ATMega2561 est le maitre du bus SPI
    - **Esclave :** L’écran GC9A01,
    - **L'Usage :** Le protocole SPI est un protocole assez rapide pour travailler et rafraichir un écran de 240x240pixels
    - **Cablage** : PB1(SCK - Horloge), PB2 (MOSI - Données), PB0 (SS/CS - Selection de l’ecran)

Les LED WS2812B utilise un protocole propriétaire de communication monofil, l’envoi des données à ces LED peut bloquer l’execution du programme et desactive les interruption du microcontrolleur

En choisissant les LED APA102 fonctionnant en SPI, la transmission de données est cadencée par la ligne d’horloge matérielle (SCK).  L’ATMega328P peut rafraichir l’affichage des LED sans aucune contrainte de temps grace au protocole SPI, lui permettant de simultanément et sans latences les capteurs

L’utilisation du protocole SPI pour l’affichage de l’ecran du TouchScreen permet aussi d’atteindre un taux de rafaichissement d’écran plus élevé et fluide avec 8MHz avec l’atm2561 contre 400kHz en I2C

## I2C **(Inter-Integrated Circuit)**

Le protocole I2C est un standard de communication bidirectionnel, il repose sur un principe d’adressage logiciel et permet de facilement reliée un microcontrolleur et différents circuits.

Contrairement au SPI qui necessite une ligne individuelle (Chip Select) par peripherique, le protocole I2C permet de connecter plusieurs dizaines de composants en parallele sur seulement deux lignes de communication:

- SCL (Serial Clock) : Signal d’horloge generé par le maitre pour cadencer la transmission des bits
- SDA (Serial Data) : Ligne de données bidirectionnelle transportant alternativement les informations du maitre vers le slave, et du slave vers le maitre

Adressage Unique: Chaque esclave I2C possède une adresse codée en binaire définie en usine ou par configuration matérielle. Le maitre commence chaque interaction en envoyant l’adresse du composant visé sur le fil SDA. Seul le composant possédant cette adresse répond, les autres ignorant la suite des paquets.

- Composant : ICM-42699-P
    - **Le Maitre** : L’ATMega328P est le maitre du bus I2C
    - **Esclave :**  ICM-42699-P
    - **L'Usage :** Le protocole I2C est
    - **Cablage** : SDA et SDL
- Composant : ecran tactile CST816D
    - **Le Maitre** : L’ATMega2561 est le maitre du bus I2C
    - **Esclave :**  CST816D
    - **L'Usage :** Le protocole I2C est
    - **Cablage** : SDA et SDL

L’adoption de l’I2C pour la gestion des capteurs de position et l’interaction tactile du TouchScreen:

- Minimisation de l’encombrement du cablage
    - L’I2C permet de connecter l’IMU et l’ecran tactile en n’utilsaint que 2 lignes de données, evitant de saturer les pins des micronctrolleur et d’alourdir le cablage flexible du bras
- Echange de données plus lent
    - Contraireemnt à l’écran LCD, le tactile et l’IMU ne transmettenet que de petit paquets de données, la vitesse de l’I2C est suffisante pour l’echange de donnée

## UART (Universal Asynchronous Receiver-Transmitter)

## ADC

## DIGITAL I/O

---

# 5. Pinout

## ATmega328p — Concentrateur capteurs + APA102

### MANDATORY

#### ALIMENTATION & PROGRAMMATION

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **VCC / GND** | Alim | 5V — 100nF par paire VCC/GND | PWR |
| **RESET** | Reset | Pull-up 10kΩ vers VCC | Reset |

---

#### CRYSTAL OSCILLATOR

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **B6 / XTAL1** | Crystal / Oscillator | External Clock |  |
| **B7 / XTAL2** | Crystal / Oscillator | External Clock |  |

---

#### ADC + MULTIPLEXER

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **C0** | CD4067 S0 | Sélection canal multiplexeur | Digital |
| **C1** | CD4067 S1 | Sélection canal multiplexeur | Digital |
| **C2** | CD4067 S2 | Sélection canal multiplexeur | Digital |
| **C3** | CD4067 S3 | Sélection canal multiplexeur | Digital |
| **ADC7** | CD4067 output | Multiplex FSR doigts + Flex | ADC |
| **ADC6** | FSR 402 poignet | Capteur de force | ADC |

---

#### I2C IMU

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **C4 / SDA** | ICM-42688 | IMU SDA | I2C |
| **C5 / SCL** | ICM-42688 | IMU SCL | I2C |

---

#### GPIO Hall

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **D6** | A3144 Hall | Capteur magnétique | GPIO |

---

#### SPI LEDs

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **B4** | APA102 ruban #1 | CS ruban 1 | GPIO CS |
| **B3 / MOSI** | APA102 DI (tous) | Data SPI — bus commun | SPI HW |
| **B5 / SCK** | APA102 CI (tous) | Clock SPI — bus commun | SPI HW |

---

#### BONUS UART

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **D0 / RX** | ATmega2561 | RX ← TX du 2561 | UART |
| **D1 / TX** | ATmega2561 | TX → RX du 2561 | UART |

---

#### RECAP

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **C0** | CD4067 S0 | Sélection canal multiplexeur | Digital |
| **C1** | CD4067 S1 | Sélection canal multiplexeur | Digital |
| **C2** | CD4067 S2 | Sélection canal multiplexeur | Digital |
| **C3** | CD4067 S3 | Sélection canal multiplexeur | Digital |
| **ADC7** | CD4067 output | Multiplex FSR doigts + Flex | ADC |
| **D6** | A3144 Hall | Capteur magnétique | Digital IN |
| **B4** | APA102 ruban #1 | CS ruban 1 | GPIO CS |
| **B3 / MOSI** | APA102 DI (tous) | Data SPI — bus commun | SPI HW |
| **B5 / SCK** | APA102 CI (tous) | Clock SPI — bus commun | SPI HW |
| **ADC6** | FSR 402 poignet | Capteur de force | ADC |
| **C4 / SDA** | ICM-42688 | IMU SDA | I2C |
| **C5 / SCL** | ICM-42688 | IMU SCL | I2C |
| **B6 / XTAL1** | Crystal / Oscillator | External Clock |  |
| **B7 / XTAL2** | Crystal / Oscillator | External Clock |  |
| **VCC / GND** | Alim | 5V — 100nF par paire VCC/GND | PWR |
| **RESET** | Reset | Pull-up 10kΩ vers VCC | Reset |

| **D0 / RX** | ATmega2561 | RX ← TX du 2561 | UART |
| --- | --- | --- | --- |
| **D1 / TX** | ATmega2561 | TX → RX du 2561 | UART |

---

## BONUS A FINIR

## ATmega2561 — Display + bus + périphériques

#### ALIMENTATION & PROGRAMMATION

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **VCC / GND** | Alim | 5V — 100nF par paire VCC/GND | PWR |
| **RESET** | Reset | Pull-up 10kΩ vers VCC | Reset |

---

#### CRYSTAL OSCILLATOR

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **XTAL1** | Crystal / Oscillator | External Clock |  |
| **XTAL2** | Crystal / Oscillator | External Clock |  |

#### UART0 — Lien vers ATmega328p

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **E0** | ATmega328p | RX ← TX du 328p | UART0 |
| **E1** | ATmega328p | TX → RX du 328p | UART0 |

#### UART1 — USB via FTDI FT232R / Bus RS-485 via MAX487

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **D2 / TXD2** | 74HC4053 | TX → FTDI RX / MAX487 DI | UART1 |
| **D3 / RXD2** | 74HC4053 | RX ← FTDI TX (1kΩ série) / MAX487 RO | UART1 |
| **D5** | 74HC4053 | Switch Digital | GPIO |
| **D4** | GPIO | MAX487 DE + ~RE (liés ensemble) | GPIO |

#### **Module TouchScreen (GC9A01 + CST816D) avec LevelShifter TXS0108EPWR**

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| PB0 | ~SS / CS | LCD CS | GPIO CS |
| PB1 | SCK | SCLK | SPI |
| PB2 | MOSI | MOSI | SPI |
| PB3 | MISO | MISO | SPI |
| PB4 | GPIO | LCD DC / RS | GPIO |
| PB5 | GPIO | LCD RST | GPIO |
| PD0 | SCL I2C | Touchscreen SCL | I2C |
| PD1 | SDA I2C | Touchscreen Data | I2C |
| PE4 | INT4 | Touchscreen Interrupt | I2C |

#### Plug & play (pouls / IR) + switch pression

| Pin | Composant | Signal / rôle | Protocole |
| --- | --- | --- | --- |
| **PE2** | INT2 | Capteur pouls / IR — signal data | INT ext. |
| **PE3** | GPIO | Détection présence connecteur (pull-up) | GPIO |
| **PE4** | INT4 | Switch pression — interrupt | INT ext. |

# 6. BOM

| Component | QTY | Prix unitaire (€) | Prix total (€) | Fournisseur |
| --- | --- | --- | --- | --- |
| **ATMega328P** | 1 | 2,26 | 2,26 | https://www.digikey.fr/fr/products/detail/microchip-technology/ATMEGA328P-AU/1832260 |
| **ATMega2561** | 1 | 13,17 | 13,17 | https://www.digikey.fr/fr/products/detail/microchip-technology/ATMEGA2561-16AU/735457 |
| **Interlink FSR 400 short (FSR doigt)** | 5 | 4,886 | 24,43 | https://www.digikey.fr/fr/product-highlight/i/interlink/fsr-400-round-force-sensing-resistor |
| **Spectra Symbol SEN-10264 2.2” (Flex Sensor)** | 4 | 10,16 | 40,64 | https://www.digikey.fr/fr/products/detail/sparkfun-electronics/10264/17829611 |
| **Interlink FSR 402 medium (FSR avant bras)** | 1 | 6,05 | 6,05 | https://www.digikey.fr/en/product-highlight/i/interlink/fsr-402-round-force-sensing-resistor |
| **Multiplexer CD4067**  | 1 | 0,79 | 0,79 | https://www.digikey.fr/fr/products/detail/texas-instruments/CD4067BM96/1691267 |
| **Capteur Effet Hall A3144** | 1 | 0,20 | 0,99 (pour 5) | https://fr.aliexpress.com/item/1005007195591707.html |
| **Aimant neodyme** | 1 | 0,99 | 0,99 (pour 20?) | https://fr.aliexpress.com/item/1005010078861287.html |
| **ICM-42688-P (IMU)** | 1 | 4,17 | 4,17 | https://www.digikey.fr/fr/products/detail/tdk-invensense/ICM-42688-P/10824934 |
|  |  |  |  |  |
| **Ruban LED APA102 (1m, 144 LED)** | 1 | 14,39 | 14,39 | https://fr.aliexpress.com/item/32969463242.html |
| **Panneau LED 16x16 (256 LEDs)** | 1 | Deprecated |  |  |
| **Panneau LED 16x32 (512 LEDs)** | 1 | Deprecated |  |  |
| **Panneau LED 16x64 (1024 LEDs)** | 1 | Deprecated |  |  |
| **Ruban LED Homemade (LEDs unitaires)** | ? |  |  |  |
|  |  |  |  |  |
| **MAX485 (DMX)** | 1 | 6,74 | 6,74 | https://www.digikey.fr/en/products/detail/analog-devices-inc-maxim-integrated/MAX485ESA/1495323 |
| **Module GC9A01 + CST816D (TouchScreen)** | 1 | 3,89 | 3,89 | https://fr.aliexpress.com/item/1005011785235078.html |
| **Module GC9A01 + CST816D (TouchScreen)** | 1 | 6,59 | 6,59 | https://fr.aliexpress.com/item/1005012175753162.html |
| **PCB** | 2 | TBH | TBH |  |
| **TXS0108E (**Level Shifter ATM - Touchscreen) | 1 | 1,46 |  | https://www.digikey.fr/fr/products/detail/texas-instruments/TXS0108EPWR/1775302 |
| **TXS0104E** (Level Shifter ATM - IMU) | 1 | 0,92 |  | https://www.digikey.fr/fr/products/detail/texas-instruments/TXS0104EPWR/1143662 |
| **Crystal 16MHz** | 2 | 0,89 | 1,78 | https://www.digikey.fr/fr/products/detail/epson/FA-238-16-0000MB-C3/2403378 |
|  |  |  |  |  |
| Condensateur céramique de découplage 100nF | 4 |  |  |  |
| Condensateur electrolityque de découplage 10mF | 1 |  |  |  |
| Self/Inductance/Bobine/FiltreLC 10 uH | 1 |  |  |  |
| Resistance 10kΩ (pour chaque FSR) | 5 |  |  |  |
|  |  |  |  |  |
| Thermistance 10kΩ | ? |  |  |  |
| Colle Thermique | 1 |  |  |  |
| Ruban Kapton | 1 |  |  |  |
|  |  |  |  |  |
| ***TOTAL*** |  |  |  |  |

# 7.Charge d’alimentation

| Component | QTY | Tension (U) | Max intensity unitaire (I) | Max intensity (I) | Power (P) |
| --- | --- | --- | --- | --- | --- |
| **ATMega328P** | 1 | 5V | 14mA | 14mA | 0,07W |
| **ATMega2561** | 1 | 5V | 14mA | 14mA | 0,07W |
| **Interlink FSR 400 short (FSR)** | 5 | 5V | ~1mA | 5mA | 0,025 W |
| **Spectra Symbol SEN-10264 2.2” (Flex Sensor)** | 4 | 5V | ~1mA | 4mA | 0,020 W |
| **Interlink FSR 402 medium (FSR)** | 1 | 5V | ~1mA | 1mA | 0,005W |
| **ICM-42688-P (IMU)** | 1 | 3.3V | ~1mA | 1mA | 0,003 W |
| **Capteur Effet Hall A3144** | 1 | 5V | ~9mA | 9mA | 0,045 W |
|  |  |  |  |  |  |
| **Ruban LED APA (1m, 144 LED)** | 1 | 5V | 144*60mA = 8,64A | 144*60mA = 8,64A | 43,2 W |
| **Panneau LED 16x16 (256 LEDs)** | 1 | 5V | 256*60mA = 15,36A | 256*60mA = 15,36A | 76,80 W |
| **Panneau LED 16x32 (512 LEDs)** | 1 | 5V | 512*60mA = 30,72A | 512*60mA = 30,72A | 153,6 W |
| **Panneau LED 16x64 (1024 LEDs** | 1 | 5V | 1024*60mA = 61,44A | 1024*60mA = 61,44A | 307,2 W |
| **Ruban LED Homemade (LEDs unitaires)** | 100 | 5V | 60mA | 100*60mA = 6A | 30 W |
|  |  |  |  |  |  |
| **MAX485 (DMX)** | 1 | 5V | 0,9mA | 0,9mA | 0,0045 W |
| **Module GC9A01 + CST816D (TouchScreen)** | 1 | 3,3V | 100mA | 100mA | 0,33 W |
|  |  |  |  |  |  |
|  |  |  |  |  |  |
