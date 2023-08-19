class Settings {
	private static _instance: Settings;

	get current() {
		if (!Settings._instance) {
			Settings._instance = new Settings();
		}

		return Settings._instance;
	}

	set(key: string, value: any) {
		localStorage.setItem(key, value);
	}

	get(key: string) {
		return localStorage.getItem(key);
	}

	remove(key: string) {
		localStorage.removeItem(key);
	}
}

export const settings = Settings.prototype.current;
