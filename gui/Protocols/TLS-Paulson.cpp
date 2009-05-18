/*
 * This is a model of the TLS version as modeled by Paulson
 *
 * The .cpp file cannot be fed into scyther directly; rather, one needs
 * to type:
 *
 * 	cpp TLS-Paulson.cpp >TLS-Paulson.spdl
 *
 * in order to generate a valid spdl file for scyther.
 *
 * This allows for macro expansion, as seen in the next part.
 *
 */
#define CERT(a) { a,pk(a) }sk(Terence)
#define MSG a,na,sid,pa,pb,nb,sid,pb,CERT(a),CERT(b),{pms}pk(b) 
#define M hash(pms,na,nb)
#define F hash(M,MSG)
#define CLIENTK hash(sid,M,na,pa,a,nb,pb,b,false)
#define SERVERK hash(sid,M,na,pa,a,nb,pb,b,true)

usertype Params, Bool, SessionID;

const hash: Function;
secret unhash: Function;
inversekeys(hash,unhash);

const pa,pb: Params;
const false,true: Bool;

const Terence: Agent;

protocol tlspaulson(a,b)
{
	role a
	{
		const na: Nonce;
		const sid: SessionID;
		const pms: Nonce;
		var nb: Nonce;
		var pb: Params;

		send_1( a,b, a,na,sid,pa );
		read_2( b,a, nb,sid,pb );
		read_3( b,a, CERT(b) );
		send_4( a,b, CERT(a) );
		send_5( a,b, { pms }pk(b) );
		send_6( a,b, { hash(nb,b,pms) }sk(a) );
		send_7( a,b, { F }CLIENTK );
		read_8( b,a, { F }SERVERK );

		claim_9a(a, SKR, SERVERK);
		claim_9b(a, SKR, CLIENTK);

	}	
	
	role b
	{
		var na: Nonce;
		var sid: SessionID;
		var pms: Nonce;
		const nb: Nonce;
		const pb: Params;

		read_1( a,b, a,na,sid,pa );
		send_2( b,a, nb,sid,pb );
		send_3( b,a, CERT(b) );
		read_4( a,b, CERT(a) );
		read_5( a,b, { pms }pk(b) );
		read_6( a,b, { hash(nb,b,pms) }sk(a) );
		read_7( a,b, { F }CLIENTK );
		send_8( b,a, { F }SERVERK );

		claim_10a(b, SKR, SERVERK);
		claim_10b(b, SKR, CLIENTK);
	}
}

