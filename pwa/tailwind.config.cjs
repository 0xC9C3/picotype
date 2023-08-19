const config = {
    content: [
        "./src/**/*.{html,js,svelte,ts}",
        "./node_modules/flowbite-svelte/**/*.{html,js,svelte,ts}",
    ],

    plugins: [
        require('flowbite/plugin')
    ],

    darkMode: 'class',

    theme: {
        extend: {
            colors: {
                primary: {
                    50: '#009a58',
                    100: '#008e4f',
                    200: '#007e45',
                    300: '#006e3b',
                    400: '#005e31',
                    500: '#004e27',
                    600: '#00441d',
                    700: '#003413',
                    800: '#002409',
                    900: '#001400',
                    950: '#000a00'
                },
                secondary: {
                    50: '#f5f5f5',
                    100: '#ebebeb',
                    200: '#d4d4d4',
                    300: '#bdbdbd',
                    400: '#8f8f8f',
                    500: '#616161',
                    600: '#575757',
                    700: '#4d4d4d',
                    800: '#434343',
                    900: '#353535',
                    950: '#2c2c2c'
                },
            }
        }
    }
};

module.exports = config;