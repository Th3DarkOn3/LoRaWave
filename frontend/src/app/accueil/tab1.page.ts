import { Component, OnInit } from '@angular/core';
import { IonicModule, ModalController } from '@ionic/angular';
import { CommonModule } from '@angular/common';
import { ExploreContainerComponent } from '../explore-container/explore-container.component';
import { ApiService } from '../api.service';
import { addIcons } from 'ionicons';
import { add, reload } from 'ionicons/icons';
import { AddDeviceFormComponent } from '../add-device-form/add-device-form.component';
import { Tab1GraphComponent } from '../tab1-graph-component/tab1-graph-component.component';
import { ModifyDeviceFormComponent } from '../modify-device-form/modify-device-form.component';
import { AlertController } from '@ionic/angular';

@Component({
  selector: 'app-tab1',
  templateUrl: 'tab1.page.html',
  styleUrls: ['tab1.page.scss'],
  standalone: true,
  imports: [IonicModule, CommonModule, ExploreContainerComponent, AddDeviceFormComponent, Tab1GraphComponent, ModifyDeviceFormComponent],
})
export class Tab1Page implements OnInit {
  devices: any[] = [];

  constructor(private apiService: ApiService, private modalCtrl: ModalController, private alertController: AlertController) {
    addIcons({add, reload});
  }

  ngOnInit() {
    this.loadDevices();
  }

  loadDevices() {
    this.apiService.getDevices().subscribe(
      (data) => {
        this.devices = data;
        console.log('Devices loaded:', this.devices);
      },
      (error) => {
        console.error('Erreur de chargement des périphériques:', error);
      }
    );
  }

  async confirmDeleteDevice(id: number) {
    const alert = await this.alertController.create({
      header: 'Confirmer la suppression',
      message: 'Êtes-vous sûr de vouloir supprimer ce périphérique?',
      buttons: [
        {
          text: 'Annuler',
          role: 'cancel',
          handler: () => {
            console.log('Suppression annulée');
          },
        },
        {
          text: 'Supprimer',
          role: 'destructive',
          handler: () => {
            this.deleteDevice(id);
          },
        },
      ],
    });

    await alert.present();
  }

  deleteDevice(id: number) {
    this.apiService.deleteDevice(id).subscribe(
      () => {
        console.log(`Device ${id} supprimé avec succès`);
        this.loadDevices(); // Ricarica la lista dopo l'eliminazione
      },
      (error) => {
        console.error('Erreur lors de la suppression du device:', error);
      }
    );
  }

  async openAddDeviceModal() {
    const modal = await this.modalCtrl.create({
      component: AddDeviceFormComponent,
    });

    modal.onDidDismiss().then((result) => {
      console.log('Modal dismissed with:', result);
    });

    return await modal.present();
  }

  async openModifyDeviceModal(device: any) {
    const modal = await this.modalCtrl.create({
      component: ModifyDeviceFormComponent,
      componentProps: { device },
    });

    modal.onDidDismiss().then((result) => {
      if (result.data === 'updated') {
        this.loadDevices();
      }
    });

    return await modal.present();
  }

  cancel() {
    return this.modalCtrl.dismiss(null, 'cancel');
  }
}