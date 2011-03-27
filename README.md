FMOD Ex Cinder Block
--------------------

C++ [Cinder](http://libcinder.org) Block to use the [FMOD Ex](http://www.fmod.org/) interactive audio library by [Roger Palà](http://rogerpala.com).

This Cinder Block is meant to be an easy way to take advantage of the powerful FMOD Ex interactive sound library. 
Mainly created to be able to play with 3D Sound environments.

The block basically consist of two classes:

* **FmodexPlayer** is a simple sound player using the Fmod Ex library based on openframeworks [ofSoundPlayer](https://github.com/openframeworks/openFrameworks/blob/master/libs/openFrameworks/sound/ofSoundPlayer.cpp).
* **Fmodex3DSoundPlayer** is a 3D Sound player using the Fmod Ex library created based on the info in the [Programming Interactivity book](http://programminginteractivity.com/) about 3D Sound by [Joshua Noble](http://thefactoryfactory.com/).

###Requirements

* Cinder 0.8.2.
* Mac OSX with xcode.

###Quick setup

First clone the repository to the cinder blocks folder:

	cd /CINDER_PATH/blocks
	git clone git://github.com/rogerpala/FMOD-Ex-Cinder-Block.git FMOD-Ex-Cinder-Block

And finally copy the libfmodex.dylib file from:

	FMOD-Ex-Cinder-Block/libs/lib/osx/libfmodex.dylib

to:

	/usr/lib/

That's it!

###Samples

* **basicFmodExSoundPlayer** is the most basic one, with a simple example on how to use the FmodexPlayer to affect the speed and pan of a sound.
* **FmodEx3DSoundPlayer** is a simple example on how to use the Fmodex3DSoundPlayer to create a 3D Sound effect. In fact, to simplify, this example it's a fake 3D sound as we just use a 2D representation of it, but it's a good start.
* **FmodExMultiple3DSoundPlayer** is an example to show how to use the Fmodex3DSoundPlayer to play multiple 3D Sounds in a 3D space. 
