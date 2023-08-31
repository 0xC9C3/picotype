import {describe, expect, it} from 'vitest';
import TypeContentPacket from "$lib/packet/client/type_content";

describe('type_content ble packet test', () => {
    it('has the packet type 2', () => {
        const p = new TypeContentPacket('A');
        const buffer = new Uint8Array(p.toBuffer());

        expect(buffer[0]).toBe(0x02);
    });

    it('has the correct length', () => {
        // should be 1 packet header + 2 packet length + body size where body = content length * 2
        const p = new TypeContentPacket('ABC');
        const buffer = new Uint8Array(p.toBuffer());

        expect(buffer.length).toBe(1 + 2 + 6)
    });

    it('has the correct body layout', () => {
        const p = new TypeContentPacket('ABC');
        console.log(p.toBuffer())
        const buffer = new Uint8Array(p.toBuffer().slice(3, p.toBuffer().byteLength + 1));

        const array = [
            0, 'A'.charCodeAt(0),
            0, 'B'.charCodeAt(0),
            0, 'C'.charCodeAt(0),
        ];

        for (let i = 0; i < buffer.length; i++) {
            expect(buffer[i]).toBe(array[i]);
        }
    });
});
