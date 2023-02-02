<h1> RF Power Detector, 2022-2023 </h1>
<h3> &ensp; <i>Team 1 - dBEngineers</i> </h3>
<br>

- - - -

For the University of Cincinnati CEAS Senior Design Capstone, 2023.

This repository houses the C++ / PlatformIO files used in the chosen microcontroller, as well as other documentation.

## Tools
<br>
<details>
<summary>Hardware</summary>
<ul>
	<li>
		Teensy 4.1
		<ul>
			<li>8MB PSRAM addon</li>
			<li>16MB NOR Flash (Planned)</li>
		</ul>
	</li>
	<li> AD8364A - Power Analyzer IC</li>
	<li> MCP33151 - 14-bit ADC</li>
	<li> SN74AXC4T774PWR - Voltage Translator</li>
	<li> W25Q128JVSIQ - 128Mb Serial Flash Memory</li>
</ul>
</details>

<br>
<details>
<summary>Software</summary>
<ul>
	<li>Visual Studio Code</li>
	<li>PlatformIO</li>
</ul>
</details>

## Group Members
<br>
<details>
<summary>Cole Trejchel</summary>
<ul>
	<li>
		Electrical Engineering Major </li>
		<li>Skills</li>
		<ul>
			<li>CAD</li>
			<li>Revit</li>
			<li>Basic Circuit Design</li>
		</ul>
	</li>
</ul>
</details>
<br>
<details>
<summary>Michael Hudepohl</summary>
<ul>
	<li>
		Electrical Engineering Major </li>
		<li>Skills</li>
		<ul>
			<li>Soldering</li>
			<li>RF Design</li>
			<li>Antenna Design</li>
		</ul>
	</li>
</ul>
</details>
<br>
<details>
<summary>Jake Parker</summary>
<ul>
	<li>
		Electrical Engineering Major </li>
		<li>Skills</li>
		<ul>
			<li>Soldering</li>
			<li>RF Design</li>
			<li>Altium Designer</li>
		</ul>
	</li>
</ul>
</details>
<br>
<details>
<summary>Nicholai Lekson</summary>
<ul>
	<li>
		Electrical Engineering Major </li>
		<li>Skills</li>
		<ul>
			<li>Embedded Development</li>
			<li>PCB Design</li>
			<li>System Design</li>
		</ul>
	</li>
</ul>
</details>

## Project Description
<br>
<details>
<summary>
The primary use of this device is to detect RF power. To power the board, the user will connect a low power DC supply. Then, either an antenna or signal generator will be connected to Port 1 through a sub-miniature version A (SMA) connection.  Once a strong enough signal is created, a root mean square voltage (VRMS) detector will create an output proportional to the input voltage. This voltage will then be processed by an analog-to-digital converter (ADC). A microcontroller will take this as an input and serially connect to a display interface through universal serial bus (USB). The display will provide information such as the VRMS, peak voltage (Vp), and the average voltage (Vavg). In possible future expansions, the board can be upgraded for other vector network analyzer (VNA) measurements such as return loss, insertion loss, complex impedance, resistance, reactance, and standing wave ratio (SWR). Port 2 can be added as well as circuitry such as signal generators, directional couplers, and switches to achieve this functionality. However, many of these new functions will still revolve around the power detector. 
</summary>
</details>

NB: This is a WORK in PROGRESS.
