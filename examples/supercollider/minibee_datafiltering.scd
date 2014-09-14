s.boot;

(
SynthDef( \acceleroFilter, {
	var outsignal = VarLag.kr(
		\xyz.kr([0.5,0.5,0.5]),
		\lagtime.kr(0.05),
		0,
		\warp.kr( Env.shapeNumber( \cub ) )
	);
	SendReply.kr( Impulse.kr( \rate.kr( 50 ) ), '/filteredAccelero', outsignal, \replyID.kr(0));
}).add;
);


~filters = 2.collect{ |i| Synth.new( \acceleroFilter, [\replyID, i ] ) };

// ~filters.do{ |it| it.free };

// ~filter.set( \warp, Env.shapeNames( \cub ) );
// ~filter.set( \lagtime, 0.05 );

(
OSCdef( \minibeedataToFilter, { |msg|
	~filters[ msg[1] - 1 ].setn( \xyz, msg.at( [6,7,8] ) );
}, "/minibee/data" );

// OSCdef( \filtereddata, { |msg|
	// x.setData( msg[2] + 11, msg.at( [3,4,5] ) );
// }, "/filteredAccelero" );
);

(
x = x ? SWDataNetwork.new;
[1,2, 11,12, 21,22].do{ |it| x.addExpected( it ) };

OSCdef( \minibeedataToNetwork, { |msg|
	x.setData( msg[1], msg.at( [6,7,8] ) );
}, "/minibee/data" );


OSCdef( \dnFilteredData, { |msg|
	x.setData( msg[2] + 11, msg.at( [3,4,5] ) );
}, "/filteredAccelero" );

x.makeGui;
);

// x.nodes[2].action = { |data| x.setData( 22, [ x.nodes[2].elapsed ] ) };


"pydongui.py".runInTerminal;

(
OSCGestureClassifier.path = "/home/nescivi/git/steim/OSCGestureClassifier/build/GestureClassification/OSCGestureClassifier"

o = OSCGestureClassifier.new( 9005 );

// NetAddr.langPort

o.start( false );
o.deltaTime = 0.025;
);

(
OSCdef( \filtereddata, { |msg|
	if ( msg[2] == 1 ){
		o.data = msg.at( [3,4,5] );
	}
}, "/filteredAccelero" );
);

(
[30].do{ |it| x.addExpected( it ) };

~repetition = SWCombineNode( 30, x, 2 )

o.actions.put( \repetition, { |v| ~repetition.set( 0, [ v ] ) } );
o.actions.put( \interval, { |v|  ~repetition.set( 1, [ v ] ) } );

x.add( \repetition, [30,0] );
x.add( \interval, [30,1] );

);
(
[31].do{ |it| x.addExpected( it ) };
~detection = SWCombineNode( 31, x, 4 );

o.actions.put( \detected, { |v| ~detection.set( 0, [ v ] ) } );
o.actions.put( \index, { |v| ~detection.set( 1, [ v ] ) } );
o.actions.put( \distance, { |v| ~detection.set( 2, [ v ] ) } );
o.actions.put( \phase, { |v| ~detection.set( 3, [ v ] ) } );

x.add( \detected, [31,0] );
x.add( \indexDetected, [31,1] );
x.add( \distanceDetected, [31,2] );
x.add( \phaseDetected, [31,3] );
);

(
[32].do{ |it| x.addExpected( it ) };
~learning = SWCombineNode( 32, x, 3 );

o.actions.put( \learned, { |v| ~learning.set( 0, [ v ] ); } );
o.actions.put( \learning, { |v1,v2| ~learning.set( 1, [ v1, v2 ] ) } );

x.add( \learnedGestures, [32,0] );
x.add( \learning, [32,1] );
x.add( \recording, [32,2] );
);


o.learn;

( // simple sonifications:

Ndef( 'periodic', { RLPF.ar( Impulse.ar( \rate.kr(10, 2) ), \freq.kr(500), \rq.kr(0.05), \amp.kr(1,5) * 10 ); } );

Ndef( \periodic ).play;

x[\repetition].action = { |v| Ndef( \periodic ).set( \amp, v ); };
x[\interval].action = { |v| Ndef( \periodic ).set( \rate, 400.0/v ); };

x[\indexDetected].action = { |v| Ndef( \periodic ).set( \freq, v*200 + 200 ); };

Ndef(\periodic ).addSpec( \rate, [1, 100, \exponential].asSpec );
);

NdefMixer.new(s);