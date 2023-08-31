import {describe, expect, it} from 'vitest';

// mock localStorage
const localStorage = {
    setItem: function () {

    },
    removeItem: function () {

    }
}

describe('local user settings', () => {
    it('can create a settings entry', () => {
        // @todo
        //settings.set('Test', 'B');
        expect(1 + 2).toBe(3);
    });
});
