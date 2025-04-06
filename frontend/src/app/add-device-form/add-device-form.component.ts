import { Component, Output, EventEmitter } from '@angular/core';
import { ApiService } from '../api.service';
import { IonicModule, ToastController, ModalController, Platform } from '@ionic/angular';
import { FormsModule } from '@angular/forms';

@Component({
  selector: 'app-add-device-form',
  templateUrl: './add-device-form.component.html',
  styleUrls: ['./add-device-form.component.scss'],
  standalone: true,
  imports: [IonicModule, FormsModule],
})
export class AddDeviceFormComponent {
  @Output() deviceAdded = new EventEmitter<void>();

  deviceName: string = '';
  latitude: number | null = null;
  longitude: number | null = null;

  nom: string | null = null;
  executionTime: number | null = null;
  targetLight: number = 0

  device: string | null = null
  interval: number | null = null
  targetTameRange: number = 1

  isDesktop: boolean = false;

    constructor(
    private apiService: ApiService,
    private toastController: ToastController,
    private modalCtrl: ModalController,
    private platform: Platform
  ) {
    this.isDesktop = this.platform.is('desktop');
  }

  async findDevice() {
    if (!this.nom || !this.executionTime) {
      const toast = await this.toastController.create({
        message: 'Veuillez entrer un nom et un temps d’exécution.',
        duration: 2000,
        color: 'warning',
      });
      await toast.present();
      return;
    }

    this.apiService.findDevice(this.nom, this.executionTime, this.targetLight).subscribe({
      next: async (response) => {
        const toast = await this.toastController.create({
          message: `Appareil trouvé - LED allumée`,
          duration: 2000,
          color: 'success',
        });
        await toast.present();
        console.log('Réponse:', response);
      },
      error: async (error) => {
        const toast = await this.toastController.create({
          message: 'Erreur lors de la recherche du dispositif.',
          duration: 2000,
          color: 'danger',
        });
        await toast.present();
        console.error('Erreur:', error);
      },
    });
  }

  async changerInterval() {
    if (!this.device || !this.interval) {
      const toast = await this.toastController.create({
        message: 'Veuillez entrer un nom et un temps d’exécution.',
        duration: 2000,
        color: 'warning',
      });
      await toast.present();
      return;
    }

    this.apiService.findDevice(this.device, this.interval, this.targetTameRange).subscribe({
      next: async (response) => {
        const toast = await this.toastController.create({
          message: `Appareil trouvé - LED allumée`,
          duration: 2000,
          color: 'success',
        });
        await toast.present();
        console.log('Réponse:', response);
      },
      error: async (error) => {
        const toast = await this.toastController.create({
          message: 'Erreur lors de la recherche du dispositif.',
          duration: 2000,
          color: 'danger',
        });
        await toast.present();
        console.error('Erreur:', error);
      },
    });
  }

  async addDevice() {
    if (this.deviceName && this.latitude !== null && this.longitude !== null) {
      const newDevice = {
        Name: this.deviceName,
        latitude: this.latitude,
        longitude: this.longitude,
        target: null
      };

      this.apiService.addDevice(newDevice).subscribe({
        next: async () => {
          const toast = await this.toastController.create({
            message: 'Appareil ajouté avec succès !',
            duration: 2000,
            color: 'success',
          });
          await toast.present();

          this.deviceAdded.emit();
          window.location.reload();
        },
        error: async () => {
          const toast = await this.toastController.create({
            message: 'Erreur lors de l\'ajout d\'un appareil.',
            duration: 2000,
            color: 'danger',
          });
          await toast.present();
        },
      });
    }
  }
}
