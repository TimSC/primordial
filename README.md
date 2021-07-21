# Primordial Life 

Copyright (C) 1996-1998 Jason Spofford, Ported to Qt5 by Tim Sheerman-Chase 2021

Primordial Life presents an environment filled with artificially living creatures called biots. Like their biological counterparts, each biot has a genetic code that serves as a blueprint for constructing it. It is this blueprint which lays the foundation for their evolution.

You will see biots battle to dominate the environment. Over time, new species will emerge while others may die out. It’s a tough life, but some "biots" got to do it.

Primordial Life runs as a screen saver or in a window. You are in control of the environment in which your biots evolve and now you can connect your environment to others.

* Ubuntu, Mint builds: https://launchpad.net/~timsc/+archive/ubuntu/primordial-life

* Windows builds: https://www.kinatomic.com/files/plife/

## Changes

Version 4.1 is the first version based on Qt5 and compiles on multiple platforms. Hosting of the project was moved to Github. https://github.com/TimSC/primordial/ While the majority of features been ported to Qt5, a few items were incomplete. This version does not include commits that exist in the SourceForge version but based on the earliest open source code, however some ideas may be incorporated in later versions.

Version 4 was a planned update that was never officially completed but instead released as open source code in 2000. The code is hosted on https://sourceforge.net/projects/primlife/ using CVS. In 2017, SourceForge eventually set their CSV repositories to read only so development stopped. This is the last version to depended on Windows MFC. The version 4 wish list, which only partially completed for version 4:

*   Motion generated not by light blue lines but by line flapping.
*   Behavior partly genetically determined, partly determined by the environment.
*   The ability for a biot to sense other biots and determine certain things about them.
*   Allow biots to control their arms.
*   The ability to edit a biot.
*   Save an individual biot, and introduce it into a different simulation.
*   Save a simulation using the file name of your choice.
*   Start a simulation from a file name.
*   Biot diseases (variable duration, transmissibility, toxicity)
*   The ability for a biot to become exclusively a male (generating little white sperm).

Version 3.16 improves video performance and adds the dreaded PURPLE PLAGUE. The purple plague reduces dense populations. The plague is transmitted biot to biot through contact. Biots can fight off the infection with energy but never gain immunity from the disease.

Over Primordial Life 3.0, version 3.1 adds networking capability. What’s that mean? It means you can connect your biot world to other biot worlds. Instead of a biot bouncing off a wall, it can go through the wall to another computer. You can connect each side of your screen to another computer (or even to your own if you’d like). The computer you connect to can also connect to other computers as well. In this manner, you can create an environment of virtually unlimited size. Watch the diversity of life increase as the space over which they can evolve increases. You can construct networked worlds of several different shapes by selecting which side connects to what computer.

You’ll find most of the details of the simulation explained in the following sections.

## What is the purpose of Primordial Life?

I wrote Primordial Life to capture the principles of evolution in an interesting and visual way. This is the fourth version of Primordial Life. 

I want to thank everyone who sent me suggestions for improving Primordial Life. Unfortunately, I didn’t have near the time to implement all of them. I hope you find the new features I selected as interesting as I do.

Thanks for trying Primordial Life.

## What do the color of the lines mean?

Biots are made up entirely of lines. Each line has a unique color and a purpose. Green lines absorb solar energy and convert it to biot energy. Without biots with green lines, all biots would surely perish. While green lines are extremely handy for capturing energy from the environment, they are vulnerable to attack from the menacing biots with red lines. Biots with red lines are considered predators, or perhaps herbivores. They steal energy from plants by attacking their green lines.

When a predator touches a green line, the predator gains energy. In addition, the plant may lose its line. If a red line is longer than a green line, the green line will be "plucked" off the biot. If there were any lines that came after the green line, they too are loss. Now, while this may sound tragic, all biots have the ability to regenerate lines they have loss, provided they have enough energy to do so. If a red line is shorter, the green line just dims, and takes much less time to regenerate.

There are also other lines that are vulnerable to attack by predators. These are the white and light blue lines. White lines are only visible when "Sexual Reproduction" has been enabled. When a white line touches any line, other than another white line, it injects the biots genetic code into the target biot. In this manner, fertilization takes place. The last biot to touch a biot before it gives birth is considered to be the father. White lines, however, are just as vulnerable as green lines.

Light blue lines were introduced in version 3.0. They are little rocket motors that pull a biot around. They fire serially at a frequency determined by the genetic code. They cause biots to rotate, or travel in an apparently erratic path throughout the environment. Light blue lines, just as white lines, are vulnerable to attack by red lines.

Biots with blue lines, or defenders, can shield themselves from a predator’s red lines. When defenders collide, no harm is done. A predator with a red line longer than the length of a blue line can "pluck" the shield of a defender and continue the attack. Likewise, a defender with a longer blue line can "pluck" the predators red line and potentially halt the assault.

When biots collide, they can flash colors to help you visually keep track of the interactions. Predators flash red when they attack. Defenders flash blue when they are actively defending. Plants, or biots with light blue or white lines flash yellow when they are being attacked.

## Just what do all those settings do?

For Each Session:

{bmc session.bmp}

By choosing Start with a new population, each time the screen saver starts up, it will start a completely new population and will commence evolving.

By choosing Stay with current population, each time the screen saver starts up, it will load the biots you were evolving previously and continue their evolution. Should you grow tired of your advanced biot civilization, you can press Erase Current Population to start with a fresh population.

Environment:

{bmc environ.bmp}

When selecting a mutation rate, you can have fun and make a high mutation rate, resulting in children biots that are bound to be somewhat different than their parents. This may make interesting shapes, but it could also hinder evolution by destroying accumulated genetic information too fast. If you pick no mutation rate, the biots will never change, although some may die. By selecting no mutation rate, in conjunction with Sexual Reproduction Only, you can watch evolution occur by only using crossover. Eventually, the population will no longer change as well.

When selecting a starting population, there are some facts to keep in mind. When using Sexual Reproduction Only, starting with a population of one will always result in extinction. I’d recommend a population of twenty or more when using Sexual Reproduction Only With Asexual Reproduction it is very interesting to start out with a population of one. It may take a bit to find a biot with high enough fitness to survive on its own, but once it does, you can observe what happens to this biot over time.

You can control the environment’s friction by making it a slick surface, or a rigid surface. For frictionless environments, once you start moving, you never stop. So even if a biot doesn’t have propulsion (light blue lines), it will likely find itself moving. When the environment’s friction is rigid, biots will not move unless they were just bumped into, or they have their own propulsion.

By increasing the Solar Intensity, you can increase the amount of energy biots can absorb from the environment. This allows biots with less green lines to survive without eating other biots. It also causes pure green biots to multiple like crazy. If you make the "Solar Intensity" too low, only mostly green biots can survive.

A biot’s Life Span is controllable. You can make a biot’s life span really short, which means it has to gain enough energy to reproduce in a very short time, or you can make a biot’s life span immortal, which means it will never die of old age.

When biot’s regenerate lost lines, it can be expensive. Via Regeneration Cost you can vary just how expensive it is. Biots will stop regenerating their lines if their energy falls below 25% of normal. You can make regeneration free, in which case, the only penalty to the biot is the fact that they loss the use of their line(s) for a period of time.

It takes a certain amount of time to regenerate lines. Via Regeneration Rate you can adjust how long it takes to regenerate. Immediate regeneration means that the biot will barely miss its missing line. Very slow regeneration means that a biot will be exposed to counter attacks for a long time to come.

On fast machines, biots can zoom around pretty fast. Via Biot Speed you can slow them down.

General Options:

{bmc general.bmp}

Primordial Life comes with sounds that you can optionally turn off. There is a sound for dying as a direct result off an attack, for starving to death, for dying of old age, and for being born. If you enable the mouse, you get a sound for energizing a biot and a sound for killing off an unlucky biot.

As mentioned previously, you can interact with Primordial Life, by selecting to use mouse. If you do this, the only way to exit the screen saver using the mouse is to right click to bring up the popup menu and select exit. You can always exit by pressing a key (like Escape) on the keyboard. Use the mouse to reward biots you like, and to terminate biots you deem unworthy of your computer screen.

Purple Plague:

{bmc plague.bmp}

The purple plaque has both duration and lethality. Prolonged duration and mild lethality will cause more biots to get the plaque. The plague happens when a mother cannot give birth due to overcrowding twice. The mother gets sick and the plague is started!

Behaviors:

{bmc behave.bmp}

Biots are fully capable of attacking their young. If you can’t bear to see this, you can prevent them from doing so. Normally, this is determined by the parents' genetic code.

Biot siblings can also fight each other. If such battles don’t fit your definition of a nice family life, feel free to prevent them from fighting. Normally this trait is determined by the children genetic code.

Reproduction:

{bmc reprod.bmp}

Biots can reproduce in two ways. They can reproduce by making an exact copy of their genetic code (subject to mutations). This is called Asexual Reproduction. They also can be fertilized by another biot, and produce offspring that are a mixture of the two parents. This is called Sexual Reproduction. You can allow both types of reproduction to occur as well, allowing evolution to determine which method of reproduction is best.

In Primordial Life 3.0, a biot belongs to a particular species, and can only fertilize those biots that belong to its species.

Color Reactions:

{bmc color.bmp}

This is an experimental feature of Primordial Life. When a biot collides with another biot, it can adjust its colors. For instance, if a predator attacks a plant, after the first attack, the plant can turn into a defender. When the newly formed defender is touched by another plant, it can change back into a plant.

There are three different ways a biot can change its color based on the color of the line it collided with. It can change the color of the line that was collided with using Allow single segment color change, or for symmetric biots, it can change the color of a line in every corresponding position using Allow radial color change. Finally, the entire biot can change color based on the color using Allow complete color change. A biots genetic code determines what method of color change it uses, subject to what you allow. The colors themselves are also stored in the genetic code.

## How do I network biots together?

Biot worlds can be connected together using TCP/IP networking. 

If you connect a fast computer to a slow computer, you may notice that some biots bounce off the wall while others make it to the slow computer. That is simply because the slow computer can not process all the biots being sent by the fast computer. This feedback helps ensure no biots are lost traveling between computers.

Computer connections are only attempted when you start the simulation, and every 512 units of time during the simulation. If you change the network settings, you’ll need to restart the simulation for the changes to take effect. Note, you can save your biots before restarting to preserve your population. When using network connections, if your population goes extinct, Primordial Life will not regenerate the population since biots may exist elsewhere.

Setting up your network connections:

{bmc network.bmp}

By choosing to interconnect biot worlds, Primordial Life will attempt to load your TCP/IP stack. If it fails to do so, it will inform of such.

By choosing to accept connections, you are allowing any other biot worlds to connect to your computer. Please note that if a computer attempts to connect to a side that is already in use, that connection attempt will fail. At least one computer must listen for connections.

You can choose to connect each side of your computer screen to another computer. You can also choose to connect your right screen to your left screen, or top to bottom, by typing in your own IP address or host name. The left side of your computer will always connect to the right side of the destination computer. Likewise, the top will always connect to the bottom. If you attempt this with dial-up network support, you might notice the dialer dialog invoke. If it does, just hit cancel. The sides should be connected even if you cancel.

Viewing world status:

{bmc status.bmp}

At any time, you can open your network status dialog by selecting "World Status" off the menu. The status of each side is listed, as well as the number of biots that have entered your world through that side, and the number that have left through that side.

In addition, your worlds population and time units are shown. Time units are incremented every time all biots get processing time. Death and birth rates are updated every 512 turns. Connections are attempted every 512 turns as well, or right after you change network settings. There is a button in your world’s status box that allows you to modify network settings on the fly. Be aware that any changes made while the simulation is running will be carried out. If you change an IP address, it will disconnect from the old address, and reconnect to the new address.

For users with a dynamic IP, other users will likely have a problem remaining connected, since your IP changes over time. You can determine your IP address by viewing your network settings. However, you must communicate that address to other users. I will be looking for a method of communicating this information over the Web.

## What does a biot’s genetic code look like?

### The Genetic Code

The genetic code consists of 10 segment genes and one trait gene. A line is made up of 1 to 10 segments, and a biot can have 1 to 8 lines.

### The Segment Genes

*    segment delta height
*    segment delta width
*    segment color(s) - an array of segment colors, only one can be expressed at a time.
*    period - how often to fire the propulsion segment
*    visible - whether or not this segment is expressed at all

### The Trait Gene

*    The default expressed line color (an index into the line gene’s line color array)
*    Color reaction strategy
*    Whether the parent can attack its children
*    Whether the sibling will attack its other siblings
*    When the children are born, whether they stay with the parent, or scatter from it
*    How many children the biot can have at once.
*    How many lines are expressed (each line consists of 1 to 10 line genes)
*    Whether or not a biots lines are mirrored (even line counts only)
*    The species identifier (0 - 15)
*    The adult scale (allows a biot to get bigger or smaller, but keeps the same form)
*    The initial rotational offset when it is born.
*    The motion frequency gene, one for each line. 

### The Species Identifier

For sexual reproduction, a species identifier is used to allow for different species to develop, thus preventing them from interbreeding. It works like this. A biot with an id of 1 may breed with a biot with an id of 2 or 0, or 1. Even if a single species develops initially, through genetic drift, they can develop into different species. A mutation of the species id may change a biots id to 2, which means it can still breed. Down the road, 2 can give way to 3, and if all the biots with id 2 die out, you are left with 1 and 3 as two species.

### Motion Frequency

While each light blue segment’s length and position effect the strength and direction of a biots propulsion. For symmetrical creatures, however, these propulsion vectors can cancel each other out, or, since the propulsion lines are fired serially, they can make the biot go in circles. The motion frequency gene can tell propulsion segments on one line to fire or skip, depending on which line it belongs. This allows symmetrical creatures to develop complicated motion patterns.

## Developing

### On Debian/Ubuntu/Mint

  sudo apt install qtcreator libqt5multimedia5-plugins qtmultimedia5-dev rapidjson-dev

## License

In the year 2000, Jason had released this code under GNU Library or Lesser General Public License version 2.0 (LGPLv2) to http://www.egroups.com/files/plife-design/ at Robert Pollak's request. In 2021, Jason and Tim Sheerman-Chase agreed to relicense the code as "LGPLv2 or later", so we can be compatible with the current GNU licenses.
