/*
 * generated by RMP Smart Objects Code Generator 0.1 
 */
 
var process_1 = new function() {
    
    this.brightness_1 = saul.get_by_name("brightness");
    this.sound_1 = saul.get_by_name("sound");
    this.buzzer_1 = saul.get_by_name("buzzer");
    this.led_1 = saul.get_by_name("led");
    
    this.sound_level;
    
    var self =  this;
    
    this.start = function () {
        var handler;
        self.led_1.write(0);
        var callback = function () {
            print("light on");
            self.activity2();
            print("return true");
            return true;
        }
        self.brightness_1.on_threshold(900, callback, saul.op.LT);
    };
    
    this.activity2 = function () {
        print("sampling sound");
        self.sound_level = self.sound_1.sample(6000);
        print("sampling sound done");
        self.split_xor3();
    };
    
    this.split_xor3 = function () {
        if (self.sound_level.max > 1000) {
            self.activity5();
        } else {
            self.activity4();
        }
    };
    
    this.activity4 = function () {
    };
    
    this.activity5 = function () {
        self.buzzer_1.write(500);
        self.led_1.write(1);
        self.activity6();
    };
    
    this.activity6 = function () {
        coap.request("http://example.runmyprocess.com/object/42/alarm", "PUT", "ON");
        self.await7();
    };
    
    this.await7 = function () {
        var callback = function () {
            self.activity8();
            return false;
        }
        coap.register_handler("cancel alarm", "PUT", callback);
    };
    
    this.activity8 = function () {
        self.buzzer_1.write(0);
        self.led_1.write(0);
        self.activity9();
    };
    
    this.activity9 = function () {
    };
    
}

process_1.start();
