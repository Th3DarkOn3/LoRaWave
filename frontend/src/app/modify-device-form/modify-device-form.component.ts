import { Component, Input, Output, EventEmitter } from '@angular/core';
import { ApiService } from '../api.service';
import { ToastController, ModalController, IonicModule } from '@ionic/angular';
import { FormsModule } from '@angular/forms';

@Component({
  selector: 'app-modify-device-form',
  templateUrl: './modify-device-form.component.html',
  styleUrls: ['./modify-device-form.component.scss'],
  standalone: true,
  imports: [IonicModule, FormsModule], // Include IonicModule and FormsModule
})
export class ModifyDeviceFormComponent {
  @Input() device: any = {};
  @Output() deviceUpdated = new EventEmitter<void>();

  constructor(
    private apiService: ApiService,
    private toastController: ToastController,
    private modalCtrl: ModalController
  ) {}

  async updateDevice() {
    if (this.device.Name && this.device.latitude !== null && this.device.longitude !== null && this.device.target !== null) {
      const updatedDevice = {
        latitude: this.device.latitude,
        longitude: this.device.longitude,
        target: this.device.target,
      };

      this.apiService.updateDevice(this.device.Name, updatedDevice).subscribe({
        next: async () => {
          const toast = await this.toastController.create({
            message: 'Device updated successfully!',
            duration: 2000,
            color: 'success',
          });
          await toast.present();

          this.deviceUpdated.emit();
          this.modalCtrl.dismiss('updated');
        },
        error: async () => {
          const toast = await this.toastController.create({
            message: 'Error updating the device.',
            duration: 2000,
            color: 'danger',
          });
          await toast.present();
        },
      });
    } else {
      const toast = await this.toastController.create({
        message: 'Please fill in all fields.',
        duration: 2000,
        color: 'warning',
      });
      await toast.present();
    }
  }

  cancel() {
    this.modalCtrl.dismiss();
  }
}