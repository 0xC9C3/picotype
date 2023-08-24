import Packet from '$lib/packet/base';

class TypeContentPacket extends Packet {
	public static readonly TYPE = 0x02;

	public content: string;

	constructor(content: string) {
		super(TypeContentPacket.TYPE, content.length);
		this.content = content;
	}

	public toBuffer(): ArrayBuffer {
		this.body = this.contentToBuffer();
		return super.toBuffer();
	}

	private contentToBuffer(): ArrayBuffer {
		const buffer = new Uint8Array(this.content.length * 2).buffer;
		const dataView = new DataView(buffer);

		for (let i = 0; i < this.content.length; i++) {
			dataView.setUint16(i * 2, this.content.charCodeAt(i));
		}

		return buffer;
	}
}

export default TypeContentPacket;
