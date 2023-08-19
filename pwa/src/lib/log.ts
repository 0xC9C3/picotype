import { settings } from '$lib/settings';

class Log {
	private static _instance: Log;

	private logEntries: any[] = [];
	private warnEntries: any[] = [];
	private errorEntries: any[] = [];

	get current() {
		if (!Log._instance) {
			Log._instance = new Log();
		}

		return Log._instance;
	}

	private get shouldLog() {
		return settings.get('debug_mode') === 'true';
	}

	l(...args: any[]) {
		if (!this.shouldLog) {
			return;
		}

		console.log(...args);
		this.logEntries.push(args);
	}

	w(...args: any[]) {
		if (!this.shouldLog) {
			return;
		}

		console.warn(...args);
		this.warnEntries.push(args);
	}

	e(...args: any[]) {
		if (!this.shouldLog) {
			return;
		}

		console.error(...args);
		this.errorEntries.push(args);
	}
}

export const log = Log.prototype.current;
