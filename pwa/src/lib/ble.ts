import { log } from '$lib/log';
import { type Writable, writable } from 'svelte/store';
import { settings } from '$lib/settings';

const SERVICE_UUID = '0000FF10-0000-1000-8000-00805F9B34FB';
const WRITE_CHARACTERISTIC_UUID = '0000FF12-0000-1000-8000-00805F9B34FB';

type Connection = {
	device: BluetoothDevice;
	server: BluetoothRemoteGATTServer;
	service: BluetoothRemoteGATTService;
	characteristic: BluetoothRemoteGATTCharacteristic;
};

class Ble {
	private static _instance: Ble;
	private _internalConnection: Connection | null = null;
	private _currentConnection: Writable<Connection | null> = writable(null);

	public get currentConnection(): Writable<Connection | null> {
		return this._currentConnection;
	}

	private set currentConnection(value: Connection | null) {
		this._currentConnection.set(value);
		this._internalConnection = value;
	}

	get current() {
		if (!Ble._instance) {
			Ble._instance = new Ble();
		}

		return Ble._instance;
	}

	get _ble() {
		// eslint-disable-next-line @typescript-eslint/ban-ts-comment
		// @ts-ignore
		return navigator?.bluetooth;
	}

	get isSupported() {
		return !!this._ble;
	}

	async addDevice() {
		const device = await this._ble.requestDevice({
			acceptAllDevices: true,
			optionalServices: [SERVICE_UUID.toLowerCase()]
		});

		return this.connect(device);
	}

	async connect(device: BluetoothDevice) {
		const server = await device.gatt?.connect();

		if (!server) {
			log.e('Server not found');
			throw new Error('Server not found');
		}

		const service = await server?.getPrimaryService(SERVICE_UUID.toLowerCase());

		if (!service) {
			log.e('Service not found');
			throw new Error('Service not found');
		}

		const characteristic = await service?.getCharacteristic(
			WRITE_CHARACTERISTIC_UUID.toLowerCase()
		);

		log.l('Connected to device', device);
		log.l('Server', server);
		log.l('Service', service);
		log.l('Characteristic', characteristic);

		if (!characteristic) {
			log.e('Characteristic not found');
			throw new Error('Characteristic not found');
		}

		// @todo: check if this is needed
		/*if (!characteristic.properties.authenticatedSignedWrites) {
            log.e('Authenticated signed writes not supported');
            throw new Error('Authenticated signed writes not supported');
        }*/

		this.currentConnection = {
			device,
			server,
			service,
			characteristic
		};

		settings.set('last_device', device.id);

		// listen for disconnect
		device.addEventListener('gattserverdisconnected', () => {
			log.l('Disconnected from device', device);
			this.currentConnection = null;
		});

		return characteristic;
	}

	disconnect() {
		if (!this._internalConnection) {
			return;
		}

		this._internalConnection.device.gatt?.disconnect();
		this.currentConnection = null;
	}

	async getDevices() {
		return await this._ble.getDevices();
	}

	async send(data: Array<number>) {
		if (!this._internalConnection) {
			throw new Error('Not connected!');
		}

		const dataBuffer = new Uint8Array(data);

		return this._internalConnection.characteristic.writeValue(dataBuffer);
	}
}

export const ble = Ble.prototype.current;
