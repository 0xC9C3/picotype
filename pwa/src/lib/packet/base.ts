class Packet {
	// byte type
	public type: number;
	public body: ArrayBuffer | null = null;

	constructor(type: number, body: ArrayBuffer | null = null) {
		this.type = type;
		this.body = body;
	}

	public get length(): number {
		return this.body?.byteLength || 0;
	}

	public static fromBuffer(buffer: ArrayBuffer): Packet {
		const dataView = new DataView(buffer);
		const type = dataView.getUint8(0);
		const body = buffer.slice(3);

		return new Packet(type, body);
	}

	public toBuffer(): ArrayBuffer {
		const buffer = new ArrayBuffer(3 + (this.body?.byteLength || 0));
		const dataView = new DataView(buffer);

		dataView.setUint8(0, this.type);
		dataView.setUint16(1, this.length);

		const body = new Uint8Array(this.body || new ArrayBuffer(0));

		for (let i = 0; i < body.byteLength; i++) {
			dataView.setUint8(i + 3, body[i]);
		}

		return buffer;
	}
}

export default Packet;
