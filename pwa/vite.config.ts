import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vitest/config';
import {fileURLToPath} from "node:url";
import {readFileSync} from "node:fs";

const path = fileURLToPath(new URL('package.json', import.meta.url));
const pkg = JSON.parse(readFileSync(path, 'utf8'));

export default defineConfig({
	plugins: [sveltekit()],
	test: {
		include: ['src/**/*.{test,spec}.{js,ts}']
	},

	define: {
		__APP_VERSION__: JSON.stringify(pkg.version)
	}
});
